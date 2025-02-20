/*
 * Copyright (c) 2020 Seagate Technology LLC
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef ZEPHYR_INCLUDE_DRIVERS_LED_LP503X_H_
#define ZEPHYR_INCLUDE_DRIVERS_LED_LP503X_H_

#define LP503X_MAX_LEDS		12
#define LP503X_COLORS_PER_LED	3

/*
 * LED channels mapping.
 */

#define LP503X_NUM_CHANNELS	52

/* Bank channels. */
#define LP503X_BANK_CHAN_BASE		0
#define LP503X_BANK_BRIGHT_CHAN		LP503X_BANK_CHAN_BASE
#define LP503X_BANK_COL1_CHAN(led)	(LP503X_BANK_CHAN_BASE + 1)
#define LP503X_BANK_COL2_CHAN(led)	(LP503X_BANK_CHAN_BASE + 2)
#define LP503X_BANK_COL3_CHAN(led)	(LP503X_BANK_CHAN_BASE + 3)

/* LED brightness channels. */
#define LP503X_LED_BRIGHT_CHAN_BASE	4
#define LP503X_LED_BRIGHT_CHAN(led)	(LP503X_LED_BRIGHT_CHAN_BASE + led)

/* LED color channels. */
#define LP503X_LED_COL_CHAN_BASE	16
#define LP503X_LED_COL1_CHAN(led)	(LP503X_LED_COL_CHAN_BASE + \
					 led * LP503X_COLORS_PER_LED)
#define LP503X_LED_COL2_CHAN(led)	(LP503X_LED_COL_CHAN_BASE + \
					 led * LP503X_COLORS_PER_LED + 1)
#define LP503X_LED_COL3_CHAN(led)	(LP503X_LED_COL_CHAN_BASE + \
					 led * LP503X_COLORS_PER_LED + 2)

#endif /* ZEPHYR_INCLUDE_DRIVERS_LED_LP503X_H_ */
