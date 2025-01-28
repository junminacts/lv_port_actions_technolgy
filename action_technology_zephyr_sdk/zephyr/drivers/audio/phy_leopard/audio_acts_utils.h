/*
 * Copyright (c) 2020 Actions Semiconductor Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Common utils for in/out audio drivers
 */
#ifndef __AUDIO_ACTS_UTILS_H__
#define __AUDIO_ACTS_UTILS_H__

#include <drivers/audio/audio_common.h>
#include <shell/shell_uart.h>

/* config mic input pin as a Universal GUIO, default 0 is disable*/
#define CONFIG_MIC_PIN_GPIO 0
/***************************************************************************************************
 * CMU_DACCLK
 */
#define CMU_DACCLK_DACFIFO0CLKEN                               BIT(24)
#define CMU_DACCLK_DACCICFIRCLOCK                              BIT(21)
#define CMU_DACCLK_DACSDMCLOCK                                 BIT(20)
#define CMU_DACCLK_DACFIR2XCLKDIV_SHIFT                        (16)
#define CMU_DACCLK_DACFIR2XCLKDIV_MASK                         (0x3 << CMU_DACCLK_DACFIR2XCLKDIV_SHIFT)
#define CMU_DACCLK_DACFIR2XCLKDIV(x)                           ((x) << CMU_DACCLK_DACFIR2XCLKDIV_SHIFT)
#define CMU_DACCLK_DACFIRCLKDIV                                (14)
#define CMU_DACCLK_DACHUMDIV                                   BIT(13)
#define CMU_DACCLK_DACCICCLCKDIV                               (12)
#define CMU_DACCLK_DACCLKSRC                                   (8)
#define CMU_DACCLK_DACCLKDIV_SHIFT                             (0)
#define CMU_DACCLK_DACCLKDIV_MASK                              (0x3 << CMU_DACCLK_DACCLKDIV_SHIFT)
#define CMU_DACCLK_DACCLKDIV(x)                                ((x) << CMU_DACCLK_DACCLKDIV_SHIFT)

/***************************************************************************************************
 * CMU_ADCCLK
 */
#define CMU_ADCCLK_ADCDEBUGEN                                   BIT(31)
#define CMU_ADCCLK_ADCSWITCHRC4M                                BIT(30)
#define CMU_ADCCLK_ADCFIFOCLKEN                                 BIT(24)
#define CMU_ADCCLK_ADCFIREN                                     BIT(23)
#define CMU_ADCCLK_ADCCICEN                                     BIT(22)
#define CMU_ADCCLK_ADCANAEN                                     BIT(21)
#define CMU_ADCCLK_ADCDMICEN                                    BIT(20)
#define CMU_ADCCLK_ADCFIRCLKRVS                                 BIT(19)
#define CMU_ADCCLK_ADCCICCLKRVS                                 BIT(18)
#define CMU_ADCCLK_ADCANACLKRVS                                 BIT(17)
#define CMU_ADCCLK_ADCDMICCLKRVS                                BIT(16)
#define CMU_ADCCLK_ADCFIRCLKDIV                                 BIT(15)
#define CMU_ADCCLK_ADCOVFSCLKDIV_SHIFT                          (12)
#define CMU_ADCCLK_ADCOVFSCLKDIV_MASK                           (0x3 << CMU_ADCCLK_ADCOVFSCLKDIV_SHIFT)
#define CMU_ADCCLK_ADCOVFSCLKDIV(x)                             ((x) << CMU_ADCCLK_ADCOVFSCLKDIV_SHIFT)
#define CMU_ADCCLK_ADCCLKSRC                                    (8)
#define CMU_ADCCLK_ADCCLKPREDIV                                 (4)
#define CMU_ADCCLK_ADCCLKDIV_SHIFT                              (0)
#define CMU_ADCCLK_ADCCLKDIV_MASK                               (0x7 << CMU_ADCCLK_ADCCLKDIV_SHIFT)
#define CMU_ADCCLK_ADCCLKDIV(x)                                 ((x) << CMU_ADCCLK_ADCCLKDIV_SHIFT)

/***************************************************************************************************
 * CMU_I2STXCLK
 */
#define CMU_I2SG0LRCLOKEN                              			BIT(31)
#define CMU_I2SG0BCLKOEN                              			BIT(30)
#define CMU_I2SG0MCLKOEN                              			BIT(29)
#define CMU_I2STX0CLKSRC                              			BIT(28)
#define CMU_I2STXCLK_I2SG0LRCLKPROC_SHIFT                     	(23)
#define CMU_I2STXCLK_I2SG0LRCLKPROC_MASK                      	(0x7 << CMU_I2STXCLK_I2SG0LRCLKPROC_SHIFT)
#define CMU_I2STXCLK_I2SG0LRCLKPROC(x)                        	((x) << CMU_I2STXCLK_I2SG0LRCLKPROC_SHIFT)
#define CMU_I2STXCLK_I2SG0LRCLKDIV_SHIFT                        (20)
#define CMU_I2STXCLK_I2SG0LRCLKDIV_MASK                         (0x7 << CMU_I2STXCLK_I2SG0LRCLKDIV_SHIFT)
#define CMU_I2STXCLK_I2SG0LRCLKDIV(x)                           ((x) << CMU_I2STXCLK_I2SG0LRCLKDIV_SHIFT)
#define CMU_I2STXCLK_I2SG0BCLKDIV_SHIFT                         (18)
#define CMU_I2STXCLK_I2SG0BCLKDIV_MASK                          (0x3 << CMU_I2STXCLK_I2SG0BCLKDIV_SHIFT)
#define CMU_I2STXCLK_I2SG0BCLKDIV(x)                            ((x) << CMU_I2STXCLK_I2SG0BCLKDIV_SHIFT)
#define CMU_I2STXCLK_I2SG0BLRCLKSRC                             BIT(16)
#define CMU_I2STXCLK_I2SG0MCLKEXTREV                            BIT(14)
#define CMU_I2STXCLK_I2SG0MCLKSRC_SHIFT                         (12)
#define CMU_I2STXCLK_I2SG0MCLKSRC_MASK                          (0x3 << CMU_I2STXCLK_I2SG0MCLKSRC_SHIFT)
#define CMU_I2STXCLK_I2SG0MCLKSRC(x)                            ((x) << CMU_I2STXCLK_I2SG0MCLKSRC_SHIFT)
#define CMU_I2STX0_FIFOCLKEN                                	BIT(11)
#define CMU_I2STXCLK_I2SG0CLKDIV_SHIFT                          (0)
#define CMU_I2STXCLK_I2SG0CLKDIV_MASK                           (0xF << CMU_I2STXCLK_I2SG0CLKDIV_SHIFT)
#define CMU_I2STXCLK_I2SG0CLKDIV(x)                             ((x) << CMU_I2STXCLK_I2SG0CLKDIV_SHIFT)

/***************************************************************************************************
 * CMU_I2SRXCLK
 */
#define CMU_I2SG1LRCLOKEN                              			BIT(31)
#define CMU_I2SG1BCLKOEN                              			BIT(30)
#define CMU_I2SG1MCLKOEN                              			BIT(29)
#define CMU_I2SRXCLK_I2SRX0CLKSRC                              	BIT(28)
#define CMU_I2SRXCLK_I2SG1LRCLKPROC_SHIFT                       (23)
#define CMU_I2SRXCLK_I2SG1LRCLKPROC_MASK                        (0x7 << CMU_I2SRXCLK_I2SG1LRCLKPROC_SHIFT)
#define CMU_I2SRXCLK_I2SG1LRCLKPROC(x)                          ((x) << CMU_I2SRXCLK_I2SG1LRCLKPROC_SHIFT)
#define CMU_I2SRXCLK_I2SG1LRCLKDIV_SHIFT                        (20)
#define CMU_I2SRXCLK_I2SG1LRCLKDIV_MASK                         (0x3 << CMU_I2SRXCLK_I2SG1LRCLKDIV_SHIFT)
#define CMU_I2SRXCLK_I2SG1LRCLKDIV(x)                           ((x) << CMU_I2SRXCLK_I2SG1LRCLKDIV_SHIFT)
#define CMU_I2SRXCLK_I2SG1BCLKDIV_SHIFT                         (18)
#define CMU_I2SRXCLK_I2SG1BCLKDIV_MASK                          (0x3 << CMU_I2SRXCLK_I2SG1BCLKDIV_SHIFT)
#define CMU_I2SRXCLK_I2SG1BCLKDIV(x)                            ((x) << CMU_I2SRXCLK_I2SG1BCLKDIV_SHIFT)
#define CMU_I2SRXCLK_I2SG1BLRCLKSRC                             BIT(16)
#define CMU_I2SRXCLK_I2SG1MCLKEXTREV                            BIT(14)
#define CMU_I2SRXCLK_I2SG1MCLKSRC_SHIFT                         (12)
#define CMU_I2SRXCLK_I2SG1MCLKSRC_MASK                          (0x3 << CMU_I2SRXCLK_I2SG1MCLKSRC_SHIFT)
#define CMU_I2SRXCLK_I2SG1MCLKSRC(x)                            ((x) << CMU_I2SRXCLK_I2SG1MCLKSRC_SHIFT)
#define CMU_I2SRX0_FIFOCLKEN                                	BIT(11)
#define CMU_I2SRXCLK_I2SG1CLKDIV_SHIFT                          (0)
#define CMU_I2SRXCLK_I2SG1CLKDIV_MASK                           (0xF << CMU_I2SRXCLK_I2SG1CLKDIV_SHIFT)
#define CMU_I2SRXCLK_I2SG1CLKDIV(x)                             ((x) << CMU_I2SRXCLK_I2SG1CLKDIV_SHIFT)

/*
 * enum a_pll_series_e
 * @brief The series of audio pll
 */
typedef enum {
	AUDIOPLL_44KSR = 0, /* 44.1K sample rate seires */
	AUDIOPLL_48KSR /* 48K sample rate series */
} a_pll_series_e;

/*
 * enum a_pll_type_e
 * @brief The audio pll type selection
 */
typedef enum {
	AUDIOPLL_TYPE_0 = 0, /* AUDIO_PLL0 */
	AUDIOPLL_TYPE_1, /* AUDIO_PLL1 */
} a_pll_type_e;

/*
 * enum a_mclk_type_e
 * @brief The rate of MCLK in the multiple of sample rate
 * @note DAC MCLK is always 256FS, and the I2S MCLK depends on BCLK (MCLK = 4BCLK)
 */
typedef enum {
    MCLK_128FS = 128,
		MCLK_192FS = 192,
    MCLK_256FS = 256,
		MCLK_384FS = 384,
    MCLK_512FS = 512,
	MCLK_768FS = 768,
	MCLK_1536FS = 1536,
} a_mclk_type_e;

#define AUDIO_DUMP_MEM(data, len) \
	{ \
		uint32_t i; \
		for (i = 0; i < len; i += 4) { \
			printk("record 0x%08x ", *(uint32_t *)&data[i]); \
			if (!((i + 4) % 16)) { \
				printk("\n"); \
			} \
		} \
		printk("\n"); \
	}

uint32_t audio_sr_khz_to_hz(audio_sr_sel_e sr_khz);
audio_sr_sel_e audio_sr_hz_to_Khz(uint32_t sr_hz);
int audio_get_pll_sample_rate_dac(uint8_t div, uint8_t fir_div,
	uint8_t fir2x_div, uint8_t cic_div, a_pll_type_e index);
int audio_get_pll_setting_dac(audio_sr_sel_e sr_khz,uint8_t *div, 
	uint8_t *fir_div,uint8_t *fir2x_div, uint8_t *cic_div,uint8_t *series);
int audio_get_pll_setting(audio_sr_sel_e sr_khz, a_mclk_type_e mclk,
	uint8_t *pre_div, uint8_t *clk_div, uint8_t *series);
int audio_get_pll_setting_i2s(uint16_t sr_khz, a_mclk_type_e mclk,
	uint8_t *div, uint8_t *series);
int audio_pll_check_config(a_pll_series_e series, uint8_t *index);
int audio_get_pll_sample_rate(a_mclk_type_e mclk, uint8_t pre_div, uint8_t clk_div, a_pll_type_e index);
int audio_get_pll_sample_rate_i2s(a_mclk_type_e mclk, uint8_t clk_div, a_pll_type_e index);
int audio_pll_get_aps(a_pll_type_e index);
int audio_pll_set_aps(a_pll_type_e index, audio_aps_level_e level);
void audio_pll_set(a_pll_type_e index, a_pll_series_e series);
void audio_pll_unset(a_pll_type_e index);
void adc_reset_control(bool enable);
#endif /* __AUDIO_ACTS_UTILS_H__ */
