//****** CN121 SDK ******//
//*** SDK Information ***//
// Clock frequency: 80MHz
// Minimum clock frequency : 32MHz
// ADC sampling rate: 250Hz
// ADC resolution£º12bit
// Power line frequency filter: 50Hz
// Optional medical standard ECG waveform
//Website: http://www.cyzur.com
//Mail: contactus@cyzur.com	
//Release Date: 2023/8/7
//Version: VB.3
//Copyright(C) CyzurTech (Shanghai) Co. Ltd. 2020-
//All rights reserved

#ifndef __USER_SYS_UTILS_H_
#define __USER_SYS_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "adc.h"
#include "ddl.h"
#include "timer3.h"
#include "gpio.h"
#include "bgr.h"
#include "uart.h"
#include "bt.h"
#include "flash.h"

#include "CN121_API.h"

#include <stdint.h>
#include <string.h>

#define DB_AOUT_Pin  GpioPin10
#define DB_AOUT_Port GpioPortB   //PC6 (AIN23)

/* Define Daughter Board(DB) LOD pin */
#define DB_LOD1_Pin   GpioPin7
#define DB_LOD1_Port  GpioPortA


#define DB_HR_PULSE_Pin   GpioPin9    // PCB OUT
#define DB_HR_PULSE_Port  GpioPortB

#define DB_PULSE_ENABLE_Pin   GpioPin6    // PCB ENABLE
#define DB_PULSE_ENABLE_Port  GpioPortB


/* Define Daughter Board(DB) RST pin */
#define DB_RST_Pin 
#define DB_RST_Port

/* Define Daughter Board(DB) SPI pins */
#define DB_CSB_Pin 	GpioPin6
#define DB_CSB_Port GpioPortA

#define DB_SCLK_Pin  GpioPin4
#define DB_SCLK_Port GpioPortA

#define DB_MISO_Pin  GpioPin3
#define DB_MISO_Port GpioPortA

#define DB_MOSI_Pin  GpioPin5
#define DB_MOSI_Port GpioPortA



/* Wrapper: WritePin Function */
void DB_RST_SET(void);
void DB_RST_RESET(void);
void DB_CSB_SET(void);
void DB_CSB_RESET(void);
void DB_SCLK_SET(void);
void DB_SCLK_RESET(void);
void DB_MOSI_SET(void);
void DB_MOSI_RESET(void);



void DB_HR_PULSE_SET(uint8_t high_valid);
void DB_HR_PULSE_RESET(uint8_t high_valid);

/* Wrapper: ReadPin Function */
uint8_t DB_MISO_HIGH(void);
uint8_t DB_MISO_LOW(void);


uint8_t DB_LOD1_LOW(void);
uint8_t DB_LOD1_HIGH(void);

/*Wrapper: Microsecond delay: us */
void CN1xx_delay_us(uint32_t nus);

/*Wrapper: SPI */
uint8_t CN1xx_SPI_ReadWrite(uint8_t* TxDataSeq,uint8_t* RxDataSeq,uint8_t SeqLen);



/***********************Start define: Peripherals associated with CN121*************************************/

/*GPIO Init*/
void GPIO_LOD_Init(void);
void GPIO_RST_Init(void);
void GPIO_SPI_Init(void);

uint8_t GPIO_PULSE_ENABLE_Init(void);
void GPIO_HR_PULSE_Init(void);


void App_ClockCfg(void);

void App_AdcPortInit(void);
void App_AdcInit(void);
void App_AdcSglCfg(void);

void App_Timer0Cfg(uint16_t u16Period);


void App_LEDPortCfg(void);

void App_UartCfg(void);






#ifdef __cplusplus
}
#endif


#endif /* USER_SYS_UTILS_H_ */
