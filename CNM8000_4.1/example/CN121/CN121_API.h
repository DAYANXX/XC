//****** CN121 SDK ******//
//*** SDK Information ***//
// Clock frequency: 80MHz
// Minimum clock frequency : 32MHz
// ADC sampling rate: 250Hz
// ADC resolution：12bit
// Power line frequency filter: 50Hz
// Optional medical standard ECG waveform
//Website: http://www.cyzur.com
//Mail: contactus@cyzur.com	
//Release Date: 2023/12/11
//Version: VC.2
//Copyright(C) CyzurTech (Shanghai) Co. Ltd. 2020-
//All rights reserved

#ifndef __CN121_API_H_
#define __CN121_API_H_


#include "CN121_utils.h"

#include <string.h>
//#include <math.h>
#include <stdlib.h>


/**
 * @brief lod status, indicates whether a lead has fallen off.
 * @note parameter can be selected as below:
 * LEAD_OFF : Leadoff Status
 * LEAD_ON  : Leadon Status
 */
extern uint8_t lod_status;
extern uint16_t noise_flag;

typedef enum {
	LEAD_OFF = 0,
	LEAD_ON
}LEAD_STATUS;

typedef enum {
	Medical_MODE = 0,
	Health_MODE
}CN121_WORKMODE;

typedef enum {
	ecgOnly    = 0,
	realRhythm = 1
}CN121_LOD_On_MODE;


typedef enum {
	TIMER_IT_PROC = 0,
	ADC_IT_PROC,
	GET_VALUE_PROC,
	GET_VALUE_Rdet_PROC
}CN121_PROC;



/**
 * @brief
 * @note   Use_For_Standard ： clinical criteria
 * @note       0: Default;
 * @note       1: meet to clinical criteria.
 * @note   ADC_RESOLUTION_12
 * @note      0: ADC RESOLUTION == 10bit;
 * @note      1: Default. ADC RESOLUTION == 12bit.
 * @note   VREF ADC Reference Voltage. Default 3.3
 * @note   VCM Voltage. Default 0.9
 */

void CN121_Parameter_Init(uint8_t Use_For_Standard, uint8_t ADC_RESOLUTION_12, float VREF, float VVCM);

/**
  * @brief  CN121 initiates procedures for selecting different application domains.
  * @note   The work mode: Medical_MODE or Health_MODE. 
  *         retval: 1 for success and 0 for failure.
  * @param  CN121_WORKMODE Enum work_mode
  * @retval uint8_t Bool value
  */
uint8_t CN121_Init(CN121_WORKMODE work_mode);

/**
  * @brief  Use this function to put the CN121 chip start work.
  */
void CN121_Start(void);

/**
  * @brief  Initialize the CN121 chip when the lod_status is lead on.
  */
void CN121_LOD_ON_Init(CN121_LOD_On_MODE init_mode);

/**
  * @brief  CN121 data processing program.
  * @note   There are three modes :TIMER_IT_PROC, ADC_IT_PROC, and GET_VALUE_PROC.
  *         Respectively used to acquire ecg signal, used in TIM interrupt, ADC interrupt.
  * @note   retval can be in 3 cases when in TIMER_IT_PROC: 0: do not start FR; 
  *         1: the start of FR; 2: the end of FR
  * @note   retval can be in 2 cases when in ADC_IT_PROC: 0: the FIFO Push is successful, 
  *         1: the FIFO is full and  FIFO Push failed.
  * @note   retval can be in 2 cases when in GET_VALUE_PROC: 0: get_value succeeded; 
  *         1: get_value failed because the FIFO state is empty; 
  *         2: get_vaue failed because the FIFO state is full,
  * @param  CN121_PROC Enum, int16_t ecg value. int16_t R point delay (relative to the current point)
  * @retval uint8_t Bool value
  */

uint8_t CN121_Data_Proc(CN121_PROC cn121_proc, int16_t *ecg, int16_t *RDelay);

/**
  * @brief  Use this function to put the CN121 chip into standby mode.
  */
uint8_t CN121_Standby(void);

/**
  * @brief  Use this function to stop the CN121 chip.
  * @note   TIM & ADC also stop.
  * @retval uint8_t Bool value
  */
uint8_t CN121_Stop(void);

/**
  * @brief  Use this function to get the size of the cache data inside the SDK.
  * @note   ADC' value push this cache.
  * @retval uint16_t the cache data
  */
uint16_t FIFO_Size(void);

uint16_t CN121_heart_Rate(void);



#endif /* CN121_CN121_API_H_ */
