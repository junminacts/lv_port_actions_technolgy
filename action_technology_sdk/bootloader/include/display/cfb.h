/*
 * Copyright (c) 2018 PHYTEC Messtechnik GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Public Monochrome Character Framebuffer API
 */

#ifndef __CFB_H__
#define __CFB_H__

#include <device.h>
#include <drivers/display.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Display Drivers
 * @addtogroup display_interfaces Display Drivers
 * @{
 * @}
 */

/**
 * @brief Public Monochrome Character Framebuffer API
 * @defgroup monochrome_character_framebuffer Monochrome Character Framebuffer
 * @ingroup display_interfaces
 * @{
 */

enum cfb_display_param {
	CFB_DISPLAY_HEIGH		= 0,
	CFB_DISPLAY_WIDTH,
	CFB_DISPLAY_PPT,
	CFB_DISPLAY_ROWS,
	CFB_DISPLAY_COLS,
};

enum cfb_font_caps {
	CFB_FONT_MONO_VPACKED		= BIT(0),
	CFB_FONT_MONO_HPACKED		= BIT(1),
	CFB_FONT_MSB_FIRST		= BIT(2),
};

struct cfb_font {
	const void *data;
	enum cfb_font_caps caps;
	uint8_t width;
	uint8_t height;
	uint8_t first_char;
	uint8_t last_char;
};

/**
 * @brief Macro for creating a font entry.
 *
 * @param _name   Name of the font entry.
 * @param _width  Width of the font in pixels
 * @param _height Height of the font in pixels.
 * @param _caps   Font capabilities.
 * @param _data   Raw data of the font.
 * @param _fc     Character mapped to first font element.
 * @param _lc     Character mapped to last font element.
 */
#define FONT_ENTRY_DEFINE(_name, _width, _height, _caps, _data, _fc, _lc)      \
	static const STRUCT_SECTION_ITERABLE(cfb_font, _name) = {	       \
		.data = _data,						       \
		.caps = _caps,						       \
		.width = _width,					       \
		.height = _height,					       \
		.first_char = _fc,					       \
		.last_char = _lc,					       \
	}

/**
 * @brief Print a string into the framebuffer.
 *
 * @param dev Pointer to device structure for driver instance
 * @param str String to print
 * @param x Position in X direction of the beginning of the string
 * @param y Position in Y direction of the beginning of the string
 *
 * @return 0 on success, negative value otherwise
 */
int cfb_print(const struct device *dev, char *str, uint16_t x, uint16_t y);

/**
 * @brief Clear framebuffer.
 *
 * @param dev Pointer to device structure for driver instance
 * @param clear_display Clear the display as well
 *
 * @return 0 on success, negative value otherwise
 */
int cfb_framebuffer_clear(const struct device *dev, bool clear_display);

/**
 * @brief Invert Pixels.
 *
 * @param dev Pointer to device structure for driver instance
 *
 * @return 0 on success, negative value otherwise
 */
int cfb_framebuffer_invert(const struct device *dev);

/**
 * @brief Finalize framebuffer and write it to display RAM,
 * invert or reorder pixels if necessary.
 *
 * @param dev Pointer to device structure for driver instance
 *
 * @return 0 on success, negative value otherwise
 */
int cfb_framebuffer_finalize(const struct device *dev);

/**
 * @brief Get display parameter.
 *
 * @param dev Pointer to device structure for driver instance
 * @param cfb_display_param One of the display parameters
 *
 * @return Display parameter value
 */
int cfb_get_display_parameter(const struct device *dev,
			      enum cfb_display_param);

/**
 * @brief Set font.
 *
 * @param dev Pointer to device structure for driver instance
 * @param idx Font index
 *
 * @return 0 on success, negative value otherwise
 */
int cfb_framebuffer_set_font(const struct device *dev, uint8_t idx);

/**
 * @brief Get font size.
 *
 * @param dev Pointer to device structure for driver instance
 * @param idx Font index
 * @param width Pointers to the variable where the font width will be stored.
 * @param height Pointers to the variable where the font height will be stored.
 *
 * @return 0 on success, negative value otherwise
 */
int cfb_get_font_size(const struct device *dev, uint8_t idx, uint8_t *width,
		      uint8_t *height);

/**
 * @brief Get number of fonts.
 *
 * @param dev Pointer to device structure for driver instance
 *
 * @return number of fonts
 */
int cfb_get_numof_fonts(const struct device *dev);

/**
 * @brief Initialize Character Framebuffer.
 *
 * @param dev Pointer to device structure for driver instance
 *
 * @return 0 on success, negative value otherwise
 */
int cfb_framebuffer_init(const struct device *dev);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __CFB_H__ */
