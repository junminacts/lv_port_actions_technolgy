/*
 * Copyright (c) 2020 Actions Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*********************
 *      INCLUDES
 *********************/
#include <assert.h>
#include "draw_util.h"

/*********************
 *      DEFINES
 *********************/
/* Must sync width LVGL lv_draw_label.c */
#define LABEL_RECOLOR_PAR_LENGTH 6
#define LV_LABEL_HINT_UPDATE_TH 1024 /*Update the "hint" if the label's y coordinates have changed more then this*/

/**********************
 *      TYPEDEFS
 **********************/
/* Must sync width LVGL lv_draw_label.c */
enum {
	CMD_STATE_WAIT,
	CMD_STATE_PAR,
	CMD_STATE_IN,
};
typedef uint8_t cmd_state_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static uint8_t hex_char_to_num(char hex);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
/*
 * split a decimal integer into digits
 *
 * @param value decimal integer value
 * @param buf buffer to store the digits
 * @param len buffer len
 * @param min_w minimum split digits (may pad 0 if integer too small)
 *
 * @retval number of split digits
 */
int split_decimal(uint32_t value, uint8_t *buf, int len, int min_w)
{
	uint32_t tmp_value = value;
	int n_digits = 0;

	/* compute digit count of value */
	do {
		n_digits++;
		tmp_value /= 10;
	} while (tmp_value > 0);

	/* return digit count if buf not available */
	if (buf == NULL || len <= 0) {
		return n_digits;
	}

	/* compute filled digit count */
	if (n_digits >= len) {
		n_digits = len;
	}

	if (min_w >= len) {
		min_w = len;
	} else {
		len = MAX(n_digits, min_w);
	}

	/* fill digits of value */
	buf += len - 1;
	min_w -= n_digits;

	for (; n_digits > 0; n_digits--) {
		*buf-- = value % 10;
		value /= 10;
	}

	/* pad zero */
	for (; min_w > 0; min_w--) {
		*buf-- = 0;
	}

	return len;
}

/*
 * draw label which support emoji
 *
 * @param draw_ctx draw context
 * @param dsc pointer to draw descriptor
 * @param coords the coordinates of the text
 * @param txt `\0` terminated text to write
 * @param hint pointer to a `lv_draw_label_hint_t` variable.
 * @param feedback pointer to a `lvgl_draw_label_result_t` variable to store draw feedback
 * @param emoji_dsc pointer to a `lvgl_draw_emoji_dsc_t` variable to store emoji info
 *
 * @retval N/A
 */
void lvgl_draw_label_with_emoji(lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc,
		const lv_area_t * coords, const char * txt, lv_draw_label_hint_t * hint,
		lvgl_draw_label_result_t *feedback, lvgl_draw_emoji_dsc_t * emoji_dsc)
{
	if (feedback) {
		feedback->line_count = 0;
		feedback->remain_txt = NULL;
	}

	if (emoji_dsc) {
		emoji_dsc->count = 0;
	}

	if(dsc->opa <= LV_OPA_MIN) return;
	if(dsc->font == NULL) {
		LV_LOG_WARN("dsc->font == NULL");
		return;
	}

	if(draw_ctx->draw_letter == NULL) {
		LV_LOG_WARN("draw->draw_letter == NULL (there is no function to draw letters)");
		return;
	}

	lv_draw_label_dsc_t dsc_mod = *dsc;

	const lv_font_t * font = dsc->font;
	int32_t w;

	/*No need to waste processor time if string is empty*/
	if(txt == NULL || txt[0] == '\0')
		return;

	lv_area_t clipped_area;
	bool clip_ok = _lv_area_intersect(&clipped_area, coords, draw_ctx->clip_area);
	if(!clip_ok) return;

	lv_text_align_t align = dsc->align;
	lv_base_dir_t base_dir = dsc->bidi_dir;

	lv_bidi_calculate_align(&align, &base_dir, txt);

	if((dsc->flag & LV_TEXT_FLAG_EXPAND) == 0) {
		/*Normally use the label's width as width*/
		w = lv_area_get_width(coords);
	}
	else {
		/*If EXPAND is enabled then not limit the text's width to the object's width*/
		lv_point_t p;
		lv_txt_get_size(&p, txt, dsc->font, dsc->letter_space, dsc->line_space, LV_COORD_MAX,
						dsc->flag);
		w = p.x;
	}

	int32_t line_height_font = lv_font_get_line_height(font);
	int32_t line_height = line_height_font + dsc->line_space;

	/*Init variables for the first line*/
	int32_t line_width = 0;
	lv_point_t pos;
	pos.x = coords->x1;
	pos.y = coords->y1;

	int32_t x_ofs = 0;
	int32_t y_ofs = 0;
	x_ofs = dsc->ofs_x;
	y_ofs = dsc->ofs_y;
	pos.y += y_ofs;

	uint32_t line_start     = 0;
	int32_t last_line_start = -1;

	/*Check the hint to use the cached info*/
	if(hint && y_ofs == 0 && coords->y1 < 0) {
		/*If the label changed too much recalculate the hint.*/
		if(LV_ABS(hint->coord_y - coords->y1) > LV_LABEL_HINT_UPDATE_TH - 2 * line_height) {
			hint->line_start = -1;
		}
		last_line_start = hint->line_start;
	}

	/*Use the hint if it's valid*/
	if(hint && last_line_start >= 0) {
		line_start = last_line_start;
		pos.y += hint->y;
	}

	uint32_t line_end = line_start + _lv_txt_get_next_line(&txt[line_start], font, dsc->letter_space, w, NULL, dsc->flag);

	/*Go the first visible line*/
	while(pos.y + line_height_font < draw_ctx->clip_area->y1) {
		/*Go to next line*/
		line_start = line_end;
		line_end += _lv_txt_get_next_line(&txt[line_start], font, dsc->letter_space, w, NULL, dsc->flag);
		pos.y += line_height;

		/*Save at the threshold coordinate*/
		if(hint && pos.y >= -LV_LABEL_HINT_UPDATE_TH && hint->line_start < 0) {
			hint->line_start = line_start;
			hint->y          = pos.y - coords->y1;
			hint->coord_y    = coords->y1;
		}

		if(txt[line_start] == '\0') return;
	}

	/*Align to middle*/
	if(align == LV_TEXT_ALIGN_CENTER) {
		line_width = lv_txt_get_width(&txt[line_start], line_end - line_start, font, dsc->letter_space, dsc->flag);

		pos.x += (lv_area_get_width(coords) - line_width) / 2;

	}
	/*Align to the right*/
	else if(align == LV_TEXT_ALIGN_RIGHT) {
		line_width = lv_txt_get_width(&txt[line_start], line_end - line_start, font, dsc->letter_space, dsc->flag);
		pos.x += lv_area_get_width(coords) - line_width;
	}
	uint32_t sel_start = dsc->sel_start;
	uint32_t sel_end = dsc->sel_end;
	if(sel_start > sel_end) {
		uint32_t tmp = sel_start;
		sel_start = sel_end;
		sel_end = tmp;
	}
	lv_draw_line_dsc_t line_dsc;

	if((dsc->decor & LV_TEXT_DECOR_UNDERLINE) || (dsc->decor & LV_TEXT_DECOR_STRIKETHROUGH)) {
		lv_draw_line_dsc_init(&line_dsc);
		line_dsc.color = dsc->color;
		line_dsc.width = font->underline_thickness ? font->underline_thickness : 1;
		line_dsc.opa = dsc->opa;
		line_dsc.blend_mode = dsc->blend_mode;
	}

	cmd_state_t cmd_state = CMD_STATE_WAIT;
	uint32_t i;
	uint32_t par_start = 0;
	lv_color_t recolor  = lv_color_black();
	lv_color_t color = lv_color_black();
	int32_t letter_w;

	lv_draw_rect_dsc_t draw_dsc_sel;
	lv_draw_rect_dsc_init(&draw_dsc_sel);
	draw_dsc_sel.bg_color = dsc->sel_bg_color;

	int32_t pos_x_start = pos.x;
	/*Write out all lines*/
	while(txt[line_start] != '\0') {
		pos.x += x_ofs;

		/*Write all letter of a line*/
		cmd_state = CMD_STATE_WAIT;
		i         = 0;
#if LV_USE_BIDI
		char * bidi_txt = lv_mem_buf_get(line_end - line_start + 1);
		_lv_bidi_process_paragraph(txt + line_start, bidi_txt, line_end - line_start, base_dir, NULL, 0);
#else
		const char * bidi_txt = txt + line_start;
#endif

		while(i < line_end - line_start) {
			uint32_t logical_char_pos = 0;
			if(sel_start != 0xFFFF && sel_end != 0xFFFF) {
#if LV_USE_BIDI
				logical_char_pos = _lv_txt_encoded_get_char_id(txt, line_start);
				uint32_t t = _lv_txt_encoded_get_char_id(bidi_txt, i);
				logical_char_pos += _lv_bidi_get_logical_pos(bidi_txt, NULL, line_end - line_start, base_dir, t, NULL);
#else
				logical_char_pos = _lv_txt_encoded_get_char_id(txt, line_start + i);
#endif
			}

			uint32_t letter;
			uint32_t letter_next;
			_lv_txt_encoded_letter_next_2(bidi_txt, &letter, &letter_next, &i);
			/*Handle the re-color command*/
			if((dsc->flag & LV_TEXT_FLAG_RECOLOR) != 0) {
				if(letter == (uint32_t)LV_TXT_COLOR_CMD[0]) {
					if(cmd_state == CMD_STATE_WAIT) { /*Start char*/
						par_start = i;
						cmd_state = CMD_STATE_PAR;
						continue;
					}
					else if(cmd_state == CMD_STATE_PAR) {   /*Other start char in parameter escaped cmd. char*/
						cmd_state = CMD_STATE_WAIT;
					}
					else if(cmd_state == CMD_STATE_IN) {   /*Command end*/
						cmd_state = CMD_STATE_WAIT;
						continue;
					}
				}

				/*Skip the color parameter and wait the space after it*/
				if(cmd_state == CMD_STATE_PAR) {
					if(letter == ' ') {
						/*Get the parameter*/
						if(i - par_start == LABEL_RECOLOR_PAR_LENGTH + 1) {
							char buf[LABEL_RECOLOR_PAR_LENGTH + 1];
							lv_memcpy_small(buf, &bidi_txt[par_start], LABEL_RECOLOR_PAR_LENGTH);
							buf[LABEL_RECOLOR_PAR_LENGTH] = '\0';
							int r, g, b;
							r       = (hex_char_to_num(buf[0]) << 4) + hex_char_to_num(buf[1]);
							g       = (hex_char_to_num(buf[2]) << 4) + hex_char_to_num(buf[3]);
							b       = (hex_char_to_num(buf[4]) << 4) + hex_char_to_num(buf[5]);
							recolor = lv_color_make(r, g, b);
						}
						else {
							recolor.full = dsc->color.full;
						}
						cmd_state = CMD_STATE_IN; /*After the parameter the text is in the command*/
					}
					continue;
				}
			}

			color = dsc->color;

			if(cmd_state == CMD_STATE_IN) color = recolor;

			letter_w = lv_font_get_glyph_width(font, letter, letter_next);

			if(sel_start != 0xFFFF && sel_end != 0xFFFF) {
				if(logical_char_pos >= sel_start && logical_char_pos < sel_end) {
					lv_area_t sel_coords;
					sel_coords.x1 = pos.x;
					sel_coords.y1 = pos.y;
					sel_coords.x2 = pos.x + letter_w + dsc->letter_space - 1;
					sel_coords.y2 = pos.y + line_height - 1;
					lv_draw_rect(draw_ctx, &draw_dsc_sel, &sel_coords);
					color = dsc->sel_color;
				}
			}

			if (letter < EMOJI_UNICODE_START) {
				dsc_mod.color = color;
				lv_draw_letter(draw_ctx, &dsc_mod, &pos, letter);
			} else if (emoji_dsc && emoji_dsc->count < emoji_dsc->max_count) {
				//store pos
				emoji_dsc->emoji[emoji_dsc->count].code = letter;
				memcpy(&emoji_dsc->emoji[emoji_dsc->count].pos, &pos, sizeof(lv_point_t));
				emoji_dsc->count++;
			}

			if(letter_w > 0) {
				pos.x += letter_w + dsc->letter_space;
			}
		}

		if(dsc->decor & LV_TEXT_DECOR_STRIKETHROUGH) {
			lv_point_t p1;
			lv_point_t p2;
			p1.x = pos_x_start;
			p1.y = pos.y + (dsc->font->line_height / 2)  + line_dsc.width / 2;
			p2.x = pos.x;
			p2.y = p1.y;
			line_dsc.color = color;
			lv_draw_line(draw_ctx, &line_dsc, &p1, &p2);
		}

		if(dsc->decor  & LV_TEXT_DECOR_UNDERLINE) {
			lv_point_t p1;
			lv_point_t p2;
			p1.x = pos_x_start;
			p1.y = pos.y + dsc->font->line_height - dsc->font->base_line - font->underline_position;
			p2.x = pos.x;
			p2.y = p1.y;
			line_dsc.color = color;
			lv_draw_line(draw_ctx, &line_dsc, &p1, &p2);
		}

#if LV_USE_BIDI
		lv_mem_buf_release(bidi_txt);
		bidi_txt = NULL;
#endif

		if (feedback && pos.y + line_height_font <= draw_ctx->clip_area->y2) {
			feedback->line_count++;
			feedback->remain_txt = &txt[line_end];
		}

		/*Go the next line position*/
		pos.y += line_height;

		if(pos.y > draw_ctx->clip_area->y2) return;

		/*Go to next line*/
		line_start = line_end;
		line_end += _lv_txt_get_next_line(&txt[line_start], font, dsc->letter_space, w, NULL, dsc->flag);

		pos.x = coords->x1;
		/*Align to middle*/
		if(align == LV_TEXT_ALIGN_CENTER) {
			line_width =
				lv_txt_get_width(&txt[line_start], line_end - line_start, font, dsc->letter_space, dsc->flag);

			pos.x += (lv_area_get_width(coords) - line_width) / 2;

		}
		/*Align to the right*/
		else if(align == LV_TEXT_ALIGN_RIGHT) {
			line_width =
				lv_txt_get_width(&txt[line_start], line_end - line_start, font, dsc->letter_space, dsc->flag);
			pos.x += lv_area_get_width(coords) - line_width;
		}
	}

	LV_ASSERT_MEM_INTEGRITY();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Convert a hexadecimal characters to a number (0..15)
 * @param hex Pointer to a hexadecimal character (0..9, A..F)
 * @return the numerical value of `hex` or 0 on error
 */
static uint8_t hex_char_to_num(char hex)
{
	uint8_t result = 0;

	if(hex >= '0' && hex <= '9') {
		result = hex - '0';
	}
	else {
		if(hex >= 'a') hex -= 'a' - 'A'; /*Convert to upper case*/

		switch(hex) {
			case 'A':
				result = 10;
				break;
			case 'B':
				result = 11;
				break;
			case 'C':
				result = 12;
				break;
			case 'D':
				result = 13;
				break;
			case 'E':
				result = 14;
				break;
			case 'F':
				result = 15;
				break;
			default:
				result = 0;
				break;
		}
	}

	return result;
}
