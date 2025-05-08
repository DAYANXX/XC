/******************************************************************************
* Copyright (C) 2017, Xiaohua Semiconductor Co.,Ltd All rights reserved.
*
* This software is owned and published by:
* Xiaohua Semiconductor Co.,Ltd ("XHSC").
*
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND
* BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
*
* This software contains source code for use with XHSC
* components. This software is licensed by XHSC to be adapted only
* for use in systems utilizing XHSC components. XHSC shall not be
* responsible for misuse or illegal use of this software for devices not
* supported herein. XHSC is providing this software "AS IS" and will
* not be responsible for issues arising from incorrect user implementation
* of the software.
*
* Disclaimer:
* XHSC MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
* REGARDING THE SOFTWARE (INCLUDING ANY ACOOMPANYING WRITTEN MATERIALS),
* ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING,
* WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED
* WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED
* WARRANTY OF NONINFRINGEMENT.
* XHSC SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT,
* NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT
* LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION,
* LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR
* INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA,
* SAVINGS OR PROFITS,
* EVEN IF Disclaimer HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
* INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED
* FROM, THE SOFTWARE.
*
* This software may be replicated in part or whole for the licensed use,
* with the restriction that this Disclaimer and Copyright notice must be
* included with each copy of this software, whether used in part or whole,
* at all times.
*/
/******************************************************************************/
/** \file main.c
 **
 ** A detailed description is available at
 ** @link Sample Group Some description @endlink
 **
 **   - 2017-05-28 LiuHL    First Version
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/

#include "gpio.h"
#include "sysctrl.h"
#include "dmac.h"
#include "CN121_utils.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                            
 ******************************************************************************/
#define TIMER_TICKS_SAMPLE 2u
#define PULSE_DUR 30u

//#define LED_DEBUG
#define UART_DEBUG

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/******************************************************************************
 * Local type definitions ('typedef')                                         
 ******************************************************************************/

/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/



volatile uint32_t u32AdcFlag;
volatile int16_t AdcRestult;

volatile uint8_t flag_time0_pulse;



/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

 

static void gen_HR_pulse(uint16_t cur_heartRate);
static void Led_SET(void);
static void Led_RESET(void);
static void Uart_send_bytes(uint8_t *data, uint8_t len);
	
/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** This sample
 **
 ******************************************************************************/
 

uint8_t u8TxData[6] = {0};

uint8_t  HR_pulse_high_valid = 1;

int32_t main(void)
{  
	
	
	App_ClockCfg();  //时钟配置初始化

	App_UartCfg();   //串口模块配置

	App_LEDPortCfg();   // LED
	
	App_AdcPortInit();
	App_AdcInit();
	App_AdcSglCfg();      // ADC

	App_Timer0Cfg(48000);   //Timer0配置初始化(周期 = 48000*(1/48M)*256 = 256ms)
 
	
	GPIO_HR_PULSE_Init(); // pulse引脚
	HR_pulse_high_valid = GPIO_PULSE_ENABLE_Init();
	
	
	// CN121初始化开始
	GPIO_LOD_Init();
	GPIO_SPI_Init();
	CN121_Parameter_Init(0,1,2.5f,0.9f);
	 
	delay1ms(200);
		
	if(CN121_Init(Medical_MODE))
	{
//		Led_SET();
	}
	else
	{
//		Led_RESET();
	}
	
	CN121_LOD_ON_Init(realRhythm);


	//开启功能

	Bt_M0_Run(TIM0); 
	
	
	uint8_t lod_pre_status = LEAD_OFF;
	int16_t ecg = 0;
	int16_t rdelay = 0;
	uint8_t i =0;
	uint8_t lod_off =0;
	
	u8TxData[0] = (uint8_t)0xff;
	u8TxData[1] = (uint8_t)0xf0;

	while (1)
	{
		if(flag_time0_pulse)
		{
			if(flag_time0_pulse == 2)
			{
				CN121_Data_Proc(TIMER_IT_PROC, NULL, NULL);
			}
			if(lod_status == LEAD_ON)
			{
					if(lod_pre_status == LEAD_OFF) {
						CN121_LOD_ON_Init(realRhythm);
					}
					lod_pre_status = LEAD_ON;
					
			} 
			else if(lod_status == LEAD_OFF)
			{
					lod_pre_status = LEAD_OFF;
			}
				
			if(lod_status == LEAD_ON)
			{
				gen_HR_pulse(CN121_heart_Rate());
				Led_RESET();
			}
			else
			{
				Led_RESET();
				DB_HR_PULSE_SET(HR_pulse_high_valid);
			}
			

			flag_time0_pulse = 0;
			
		}

		if(lod_status == LEAD_ON)
		{
			if(!CN121_Data_Proc(GET_VALUE_Rdet_PROC , &ecg, &rdelay))
			{	
				uint16_t HR = CN121_heart_Rate();
				u8TxData[2] = (uint8_t)ecg;
				u8TxData[3] = (uint8_t)(ecg>>8);
				u8TxData[4] = (uint8_t)HR;
				u8TxData[5] = (uint8_t)(HR>>8);
				Uart_send_bytes(u8TxData,6);

//				if(rdelay)
//				{
//					uint16_t HR = CN121_heart_Rate();
//					u8TxData[0] = (uint8_t)HR;
//					Uart_send_bytes(u8TxData,1);
//				}
			}
		}
		else
		{
			lod_off++;
//			u8TxData[2] = (uint8_t)lod_off;
//			u8TxData[3] = (uint8_t)(lod_off>>8);
//			u8TxData[4] = (uint8_t)lod_off;
//			u8TxData[5] = (uint8_t)(lod_off>>8);
//			Uart_send_bytes(u8TxData,6);
//			delay1ms(5);
		}
	}
}


void Tim0_IRQHandler(void)
{
	static  uint8_t i =0;
    //Timer0 模式0 溢出中断
    if(TRUE == Bt_GetIntFlag(TIM0, BtUevIrq))
    {
			if(i == 0)
			{
				Adc_SGL_Start();  
				i = 1;
				flag_time0_pulse = 2;
			}
			else if (i == 1)
			{
				i = 0;
				flag_time0_pulse = 1;
			}
			
			Bt_ClearIntFlag(TIM0,BtUevIrq); //中断标志清零
    }
}



 ///< ADC 中断服务程序
void Adc_IRQHandler(void)
{    
    if(TRUE == Adc_GetIrqStatus(AdcMskIrqSgl))
    {

        Adc_ClrIrqStatus(AdcMskIrqSgl);       ///< 清除中断标志位
			
				int16_t adc_val_temp = (int16_t)Adc_GetSglResult();
				CN121_Data_Proc(ADC_IT_PROC, &adc_val_temp, NULL);
			
        Adc_SGL_Stop();                       ///< ADC 单次转换停止
    }
}




void gen_HR_pulse(uint16_t cur_heartRate)
{
	static uint16_t pre_heartRate = 0;
	static uint16_t RRi = 0;
	static uint16_t time_count  = 0;
	static uint8_t pulse_high  = 0;
		

	if((pre_heartRate ==0) && (cur_heartRate == 0))
	{
		DB_HR_PULSE_RESET(HR_pulse_high_valid);
		return;
	}
	
	time_count += TIMER_TICKS_SAMPLE;
	if((pre_heartRate != cur_heartRate) && (cur_heartRate != 0))
	{
		pre_heartRate = cur_heartRate;
		RRi = 60000 / pre_heartRate;  //60000 / pre_heartRate *2
		
	}
	
	if(pulse_high && time_count >= PULSE_DUR)
	{
		DB_HR_PULSE_RESET(HR_pulse_high_valid);
		pulse_high = 0;
		time_count = 0;
	}
	else if(!pulse_high && time_count >= (RRi - PULSE_DUR))
	{
		DB_HR_PULSE_SET(HR_pulse_high_valid);
		pulse_high = 1;
		time_count = 0;
	}
}




void Led_SET(void)
{
#ifdef LED_DEBUG
	Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, TRUE); 
#endif
}

void Led_RESET(void)
{
#ifdef LED_DEBUG
	Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, FALSE); 
#endif
}


void Uart_send_bytes(uint8_t *data, uint8_t len)
{
	
#ifdef UART_DEBUG
		for(uint8_t i=0;i<len;i++)
		{
				Uart_SendDataPoll(M0P_UART0,data[i]); //查询方式发送数据
		}
#endif
		
}


/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


