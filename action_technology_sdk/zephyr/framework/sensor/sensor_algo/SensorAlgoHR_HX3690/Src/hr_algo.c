/*******************************************************************************
 * @file    hr_algo.c
 * @author  MEMS Application Team
 * @version V1.0
 * @date    2021-5-25
 * @brief   sensor algorithm api
*******************************************************************************/

/******************************************************************************/
//includes
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hr_algo.h>
#include <hx3690l.h>

/******************************************************************************/
//constants
/******************************************************************************/
#define DBG(...)			printk(__VA_ARGS__)
//#define DBG(...)			do {} while (0)

/******************************************************************************/
//variables
/******************************************************************************/
/* hr os api */
hr_os_api_t hr_os_api = {0};

extern bool timer_40ms_en;
extern bool timer_320ms_en;
extern uint8_t timer_320ms_cnt;


/******************************************************************************/
//functions
/******************************************************************************/
/* Init sensor algo */
int hr_algo_init(const hr_os_api_t *api)
{
	// init os api
	if (api == NULL) {
		return -1;
	}
	hr_os_api = *api;
	
	hx3690l_ppg_off();
	return 0;
}

/* Start hr sensor */
int hr_algo_start(int mode)
{
	hx3690l_init(HRS_MODE);
	DBG("hx3690_start %d \r\n",HRS_MODE);
	return 0;
}

/* Stop hr sensor */
int hr_algo_stop(void)
{
	hx3690l_ppg_off();
	return 0;
}

/* Process data through call-back handler */
int hr_algo_process(void)
{
	if(timer_40ms_en)
		gsen_read_timeout_handler(0);
	if(timer_320ms_en){
		timer_320ms_cnt ++;
		if(timer_320ms_cnt == 8){
			timer_320ms_cnt = 0;
			heart_rate_meas_timeout_handler(0);
		}
	}
	return 0;
}
