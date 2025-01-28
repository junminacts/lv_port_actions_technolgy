/*******************************************************************************
 * @file    gh3011_port.c
 * @author  MEMS Application Team
 * @version V1.0
 * @date    2021-5-25
 * @brief   sensor algorithm api
*******************************************************************************/

/******************************************************************************/
//includes
/******************************************************************************/
#include <stdlib.h>
#include "gh3011_example_common.h"
#include "hr_algo.h"

/******************************************************************************/
//constants
/******************************************************************************/
//#define DBG(...)			printk(__VA_ARGS__)
#define DBG(...)			do {} while (0)

/******************************************************************************/
//variables
/******************************************************************************/
/* hr os api */
extern hr_os_api_t hr_os_api;

/******************************************************************************/
//porting functions
/******************************************************************************/

/// i2c for gh30x init
void hal_gh30x_i2c_init(void)
{
	// code implement by user
}

/// i2c for gh30x wrtie
uint8_t hal_gh30x_i2c_write(uint8_t device_id, const uint8_t write_buffer[], uint16_t length)
{
	int ret = -1;
	uint16_t reg;
	
	reg = (write_buffer[0] << 8) | write_buffer[1];
	ret = sensor_hal_write(ID_HR, reg, (uint8_t*)write_buffer + 2, length - 2);
	
	return (ret == 0) ? GH30X_EXAMPLE_OK_VAL : GH30X_EXAMPLE_ERR_VAL;
}

/// i2c for gh30x read
uint8_t hal_gh30x_i2c_read(uint8_t device_id, const uint8_t write_buffer[], uint16_t write_length, uint8_t read_buffer[], uint16_t read_length)
{
	int ret = -1;
	uint16_t reg;

	reg = (write_buffer[0] << 8) | write_buffer[1];
	ret = sensor_hal_read(ID_HR, reg, (uint8_t*)read_buffer, read_length);
	
	return (ret == 0) ? GH30X_EXAMPLE_OK_VAL : GH30X_EXAMPLE_ERR_VAL;
}

/* gh30x spi interface */

/// spi for gh30x init
void hal_gh30x_spi_init(void)
{
	// code implement by user
}

/// spi for gh30x wrtie
uint8_t hal_gh30x_spi_write(const uint8_t write_buffer[], uint16_t length)
{
	uint8_t ret = 1;
	// code implement by user
	return ret;
}

/// spi for gh30x read
uint8_t hal_gh30x_spi_read(uint8_t read_buffer[], uint16_t length)
{
	uint8_t ret = 1;
	// code implement by user
	return ret;
}

/// spi cs set low for gh30x
void hal_gh30x_spi_cs_set_low(void)
{
	// code implement by user
}

/// spi cs set high for gh30x
void hal_gh30x_spi_cs_set_high(void)
{
	// code implement by user
}


/* delay */

/// delay us
void hal_gh30x_delay_us(uint16_t us_cnt)
{
	// code implement by user
}

/* gsensor driver */

/// gsensor motion detect mode flag
bool gsensor_drv_motion_det_mode = false;

/// gsensor init
int8_t gsensor_drv_init(void)
{
	int8_t ret = GH30X_EXAMPLE_OK_VAL;
	gsensor_drv_motion_det_mode = false;
	// code implement by user
	/* if enable all func equal 25Hz, should config > 25Hz;
	but if enable have 100hz, should config to > 100hz. if not, feeback to GOODIX!!!
	*/
	return ret;
}

/// gsensor enter normal mode
void gsensor_drv_enter_normal_mode(void)
{
	// code implement by user
	gsensor_drv_motion_det_mode = false;
}

/// gsensor enter fifo mode
void gsensor_drv_enter_fifo_mode(void)
{
	// code implement by user
	gsensor_drv_motion_det_mode = false;
}

/// gsensor enter motion det mode
void gsensor_drv_enter_motion_det_mode(void)
{
	// code implement by user
	/* if enable motion det mode that call @ref hal_gsensor_drv_int1_handler when motion generate irq
		e.g. 1. (hardware) use gsensor motion detect module with reg config
			 2. (software) gsensor enter normal mode, then define 30ms timer get gsensor rawdata,
			 	if now total acceleration(sqrt(x*x+y*y+z*z)) - last total acceleration >= 30 (0.05g @512Lsb/g) as motion
				generate that call @ref hal_gsensor_drv_int1_handler
	*/
	gsensor_drv_motion_det_mode = true;
}

/// gsensor get fifo data
void gsensor_drv_get_fifo_data(ST_GS_DATA_TYPE gsensor_buffer[], uint16_t *gsensor_buffer_index, uint16_t gsensor_max_len)
{
	// code implement by user
}

/// gsensor clear buffer 
void gsensor_drv_clear_buffer(ST_GS_DATA_TYPE gsensor_buffer[], uint16_t *gsensor_buffer_index, uint16_t gsensor_buffer_len)
{
    if ((gsensor_buffer != NULL) && (gsensor_buffer_index != NULL))
    {
        memset(gsensor_buffer, 0, sizeof(ST_GS_DATA_TYPE) * gsensor_buffer_len);
        *gsensor_buffer_index = 0;
    }
}

/// gsensor get data
void gsensor_drv_get_data(ST_GS_DATA_TYPE *gsensor_data_ptr)
{
	// code implement by user
}


/* int */

/// gh30x int handler
void hal_gh30x_int_handler(void)
{
    gh30x_int_msg_handler();
}

/// gh30x int pin init, should config as rise edge trigger
void hal_gh30x_int_init(void)
{
	// code implement by user
    // must register func hal_gh30x_int_handler as callback
}

/// gsensor int handler
void hal_gsensor_drv_int1_handler(void)
{
// code implement by user
	if (gsensor_drv_motion_det_mode)
	{
		gsensor_motion_has_detect();
	}
	else
	{
		/* if using gsensor fifo mode, should get data by fifo int 
			* e.g. gsensor_read_fifo_data();   
		*/
		gsensor_read_fifo_data(); // got fifo int
	}
}

/// gsensor int1 init, should config as rise edge trigger
void hal_gsensor_int1_init(void)
{
	// code implement by user
	// must register func hal_gsensor_drv_int1_handler as callback
    
	/* if using gsensor fifo mode,
	and gsensor fifo depth is not enough to store 1 second data,
	please connect gsensor fifo interrupt to the host,
	or if using gsensor motion detect mode(e.g  motion interrupt response by 0.5G * 5counts),
	and implement this function to receive gsensor interrupt.
	*/ 
}


/* handle algorithm result */

#ifdef __HBD_API_EX__
/// handle hb mode algorithm result
void handle_hb_mode_result(ST_HB_RES *stHbRes, GS32 rawdata_ptr[][DBG_MCU_MODE_PKG_LEN], GU16 rawdata_len)
{
	// code implement by user
	DBG("------------------------------------------------------\r\n");
	DBG("HeartRate: %d, HeartRate Level:%d, HRV: %d\r\n", stHbRes->uchHbValue, 0, 0);
	
	hr_os_api.hb_handler(stHbRes->uchHbValue, stHbRes->uchAccuracyLevel, stHbRes->uchWearingQuality);
	switch (stHbRes->uchWearingState) {
		case WEAR_STATUS_DETECTING:
			DBG("wear status detecting...\r\n");
			break;
		case WEAR_STATUS_WEAR:
			DBG("wear status: WEAR\r\n");
			hr_os_api.wear_handler(1);
			break;
		case WEAR_STATUS_UNWEAR:
			DBG("wear status: UNWEAR\r\n");
			hr_os_api.wear_handler(0);
			break;
		case WEAR_STATUS_ALMOST_UNWEAR:
			DBG("wear status: ALMOST UNWEAR\r\n");
			break;
		default:
			break;
	}
	
	DBG("------------------------------------------------------\r\n\n");
	
}
#else
/// handle hb mode algorithm result
void handle_hb_mode_result(GU8 hb_val, GU8 hb_lvl_val, GU8 wearing_state_val, GU16 rr_val, GS32 rawdata_ptr[][DBG_MCU_MODE_PKG_LEN], 
									GU16 rawdata_len)
{
	// code implement by user
}
#endif

#if (__SPO2_DET_SUPPORT__)
#ifdef __HBD_API_EX__
/// handle spo2 mode algorithm result
void handle_spo2_mode_result(ST_SPO2_RES *pstSpo2Res, GS32 rawdata_ptr[][DBG_MCU_MODE_PKG_LEN], GU16 rawdata_len)
{
	// code implement by user
	DBG("------------------------------------------------------\r\n");
	
	/* DBG("SPO2: %d, SPO2 Level: %d\r\nHeartRate: %d, HeartRate Level:%d\r\nHRV: %d, HRV Level: %d\r\n", \
	 		spo2_val, spo2_lvl_val, hb_val, hb_lvl_val, rr_val[rr_cnt], rr_lvl_val);
	*/
	switch (pstSpo2Res->uchWearingState) {
		case WEAR_STATUS_DETECTING:
			DBG("wear status detecting...\r\n");
			break;
		case WEAR_STATUS_WEAR:
			DBG("wear status: WEAR\r\n");
			hr_os_api.wear_handler(1);
			break;
		case WEAR_STATUS_UNWEAR:
			DBG("wear status: UNWEAR\r\n");
			hr_os_api.wear_handler(0);
			break;
		case WEAR_STATUS_ALMOST_UNWEAR:
			DBG("wear status: ALMOST UNWEAR\r\n");
			break;
		default:
			break;
	}
	
	DBG("------------------------------------------------------\r\n\n");
	
	hr_os_api.spo2_handler(pstSpo2Res->uchSpo2, pstSpo2Res->usSpo2RVal, pstSpo2Res->uchHbValue, pstSpo2Res->uchHbConfidentLvl, pstSpo2Res->usHrvRRVal,  pstSpo2Res->uchHrvConfidentLvl, pstSpo2Res->uchHrvcnt, pstSpo2Res->usSpo2RVal);
}
#else
/// handle spo2 mode algorithm result
void handle_spo2_mode_result(uint8_t spo2_val, uint8_t spo2_lvl_val, uint8_t hb_val, uint8_t hb_lvl_val, uint16_t rr_val[4], uint8_t rr_lvl_val, uint8_t rr_cnt, 
									uint16_t spo2_r_val, uint8_t wearing_state_val, GS32 rawdata_ptr[][DBG_MCU_MODE_PKG_LEN], uint16_t rawdata_len)
{
	// code implement by user
}
#endif
#endif

/// handle hrv mode algorithm result
void handle_hrv_mode_result(GU16 rr_val_arr[HRV_MODE_RES_MAX_CNT], GU8 rr_val_cnt, GU8 rr_lvl, GS32 rawdata_ptr[][DBG_MCU_MODE_PKG_LEN], GU16 rawdata_len)
{
	// code implement by user
	DBG("------------------------------------------------------\r\n");
	DBG("HRV: %d, HRV Level: %d\r\n", rr_val_arr[rr_val_cnt], rr_lvl);
	DBG("------------------------------------------------------\r\n\n");
	
	hr_os_api.hrv_handler(rr_val_arr, rr_val_cnt, rr_lvl);
}

#if (__BPF_DET_SUPPORT__)
/// handle bpf mode algorithm result
void handle_bpf_mode_result(GU16 res_refreshed, ST_BPF_RES *pstBPFRes, GS32 rawdata_ptr[][DBG_MCU_MODE_PKG_LEN], GU16 rawdata_len)
{
	// code implement by user
}
#endif

/// handle getrawdata only mode result
void handle_getrawdata_mode_result(uint8_t wearing_state_val, GS32 rawdata[__GET_RAWDATA_BUF_LEN__ ][2], uint16_t rawdata_len)
{
    // code implement by user
}

/// handle wear status result
void handle_wear_status_result(uint8_t wearing_state_val)
{
	DBG("------------------------------------------------------\r\n");
	
	// code implement by user
	switch (wearing_state_val) {
		case WEAR_STATUS_DETECTING:
			DBG("wear status detecting...\r\n");
			break;
		case WEAR_STATUS_WEAR:
			DBG("wear status: WEAR\r\n");
			hr_os_api.wear_handler(1);
			break;
		case WEAR_STATUS_UNWEAR:
			DBG("wear status: UNWEAR\r\n");
			hr_os_api.wear_handler(0);
			break;
		case WEAR_STATUS_ALMOST_UNWEAR:
			DBG("wear status: ALMOST UNWEAR\r\n");
			break;
		default:
			break;
	}
	
	DBG("------------------------------------------------------\r\n\n");
}

/// handle wear status result, otp_res: <0=> ok , <1=> err , <2=> para err
void handle_system_test_result(uint8_t test_res,uint8_t led_num)
{
	// code implement by user
}

/// handle wear status result, led_num: {0-2};os_res: <0=> ok , <1=> rawdata err , <2=> noise err , <3=> para err
void handle_before_system_os_test()
{
    //code implement by user
}

/* ble */

/// send value via heartrate profile
void ble_module_send_heartrate(uint32_t heartrate)
{
	// code implement by user
}

/// add value to heartrate profile
void ble_module_add_rr_interval(uint16_t rr_interval_arr[], uint8_t cnt)
{
	// code implement by user
}

/// send string via through profile
uint8_t ble_module_send_data_via_gdcs(uint8_t string[], uint8_t length)
{
	uint8_t ret = GH30X_EXAMPLE_OK_VAL;
	// code implement by user
	return ret;
}

/// recv data via through profile
void ble_module_recv_data_via_gdcs(uint8_t *data, uint8_t length)
{
	gh30x_app_cmd_parse(data, length);
}


/* timer */

/// gh30x fifo int timeout timer handler
void hal_gh30x_fifo_int_timeout_timer_handler(void)
{
	gh30x_fifo_int_timeout_msg_handler();
}

/// gh30x fifo int timeout timer start
void hal_gh30x_fifo_int_timeout_timer_start(void)
{
    // code implement by user
}

/// gh30x fifo int timeout timer stop
void hal_gh30x_fifo_int_timeout_timer_stop(void)
{
    // code implement by user
}

/// gh30x fifo int timeout timer init 
void hal_gh30x_fifo_int_timeout_timer_init(void)
{
	// code implement by user
	// must register func gh30x_fifo_int_timeout_timer_handler as callback
	/* should setup timer interval with fifo int freq(e.g. 1s fifo int setup 1080ms timer)
	*/
}

#if ((__USE_GOODIX_APP__) && (__ALGO_CALC_WITH_DBG_DATA__))
/// ble repeat send data timer handler
void ble_module_repeat_send_timer_handler(void)
{
    // code implement by user
}

/// ble repeat send data timer start
void ble_module_repeat_send_timer_start(void)
{
    // code implement by user
}

/// ble repeat send data timer stop
void ble_module_repeat_send_timer_stop(void)
{
    // code implement by user
}

/// ble repeat send data timer init 
void ble_module_repeat_send_timer_init(void)
{
    // code implement by user
    // must register func ble_module_repeat_send_timer_handler as callback
	/* should setup 100ms timer and ble connect interval should < 100ms*/
}
#endif

/* uart */

/// recv data via uart
void uart_module_recv_data(uint8_t *data, uint8_t length)
{
	gh30x_uart_cmd_parse(data, length);
}

/// send value via uart
uint8_t uart_module_send_data(uint8_t string[], uint8_t length)
{
	uint8_t ret = GH30X_EXAMPLE_OK_VAL;
	// code implement by user
	return ret;
}


/* handle cmd with all ctrl cmd @ref EM_COMM_CMD_TYPE */
void handle_goodix_communicate_cmd(EM_COMM_CMD_TYPE cmd_type)
{
	// code implement by user
}


/* log */

/// print dbg log
void example_dbg_log(char *log_string)
{
	// code implement by user
	DBG("%s", log_string);
}

#if (__HBD_USE_DYN_MEM__)
void *hal_gh30x_memory_malloc(GS16 size)
{
    // code implement by user    
}

void hal_gh30x_memory_free(void *pmem)
{
    // code implement by user
}
#endif

/********END OF FILE********* Copyright (c) 2003 - 2020, Goodix Co., Ltd. ********/
