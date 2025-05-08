
#include "CN121_utils.h"




void CN1xx_delay_us(uint32_t num)
{
	delay100us(num);
}

uint8_t CN1xx_SPI_ReadWrite(uint8_t* TxDataSeq,uint8_t* RxDataSeq,uint8_t SeqLen)
{	
//			spi_xfer_done = false;
//			APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, TxDataSeq, SeqLen, RxDataSeq, SeqLen));
//		while(!spi_xfer_done)
//			;
	uint8_t hw_spi = 0; // if use hardware spi, make sure hw_spi = 1;
	return hw_spi;
}
	
void GPIO_LOD_Init(void)
{
	  stc_gpio_cfg_t stcGpioCfg;
	    ///< ��GPIO����ʱ���ſ�
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    ///< �˿ڷ�������->����
    stcGpioCfg.enDir = GpioDirIn;
    ///< �˿�������������->����������
    stcGpioCfg.enDrv = GpioDrvL;
    ///< �˿�����������->��
    stcGpioCfg.enPu = GpioPuDisable;
    stcGpioCfg.enPd = GpioPdDisable;
    ///< �˿ڿ�©�������->��©����ر�
    stcGpioCfg.enOD = GpioOdDisable;
    ///< �˿�����/���ֵ�Ĵ������߿���ģʽ����->AHB
    stcGpioCfg.enCtrlMode = GpioAHB;
		
    ///< GPIO IO USER KEY��ʼ��
    Gpio_Init(DB_LOD1_Port, DB_LOD1_Pin, &stcGpioCfg); 
}


void GPIO_RST_Init(void)
{
	
}

void GPIO_SPI_Init(void)
{
		
	  stc_gpio_cfg_t stcGpioCfg;
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 
    
    stcGpioCfg.enDir = GpioDirOut;
    
    Gpio_ClrIO(DB_SCLK_Port, DB_SCLK_Pin);
    Gpio_Init(DB_SCLK_Port, DB_SCLK_Pin, &stcGpioCfg);
	
	  Gpio_ClrIO(DB_MOSI_Port, DB_MOSI_Pin);
    Gpio_Init(DB_MOSI_Port, DB_MOSI_Pin, &stcGpioCfg);
		
	  Gpio_ClrIO(DB_CSB_Port, DB_CSB_Pin);
    Gpio_Init(DB_CSB_Port, DB_CSB_Pin, &stcGpioCfg);
	
	
	
		stc_gpio_cfg_t stcGpioInCfg;
    stcGpioInCfg.enDir = GpioDirIn;
    stcGpioInCfg.enDrv = GpioDrvL;
    stcGpioInCfg.enPu = GpioPuDisable;
    stcGpioInCfg.enPd = GpioPdDisable;
    stcGpioInCfg.enOD = GpioOdDisable;
    stcGpioInCfg.enCtrlMode = GpioAHB;
		
    Gpio_Init(DB_MISO_Port, DB_MISO_Pin, &stcGpioInCfg); 
		
    
}


uint8_t DB_LOD1_LOW(void)
{
	return (FALSE == Gpio_GetInputIO(DB_LOD1_Port, DB_LOD1_Pin));
}
uint8_t DB_LOD1_HIGH(void)
{
	return (TRUE == Gpio_GetInputIO(DB_LOD1_Port, DB_LOD1_Pin));
}

uint8_t DB_MISO_HIGH(void)
{
	return (TRUE == Gpio_GetInputIO(DB_MISO_Port, DB_MISO_Pin));
}
uint8_t DB_MISO_LOW(void)
{
	return (FALSE == Gpio_GetInputIO(DB_MISO_Port, DB_MISO_Pin));
}

void DB_RST_SET(void)
{
	
}
void DB_RST_RESET(void)
{
	
}

void DB_CSB_SET(void)
{
	Gpio_SetIO(DB_CSB_Port, DB_CSB_Pin);
}
void DB_CSB_RESET(void)	
{
	Gpio_ClrIO(DB_CSB_Port, DB_CSB_Pin);
}
void DB_SCLK_SET(void)	
{
	Gpio_SetIO(DB_SCLK_Port, DB_SCLK_Pin);
}
void DB_SCLK_RESET(void)	
{
	Gpio_ClrIO(DB_SCLK_Port, DB_SCLK_Pin);
}
void DB_MOSI_SET(void)	
{	
	Gpio_SetIO(DB_MOSI_Port, DB_MOSI_Pin);
}
void DB_MOSI_RESET(void)
{	
	Gpio_ClrIO(DB_MOSI_Port, DB_MOSI_Pin);
}


void DB_HR_PULSE_SET(uint8_t high_valid)	
{	
	if(high_valid) {
		Gpio_ClrIO(DB_HR_PULSE_Port, DB_HR_PULSE_Pin);
	}
	else {
		Gpio_SetIO(DB_HR_PULSE_Port, DB_HR_PULSE_Pin);
	}
	
}
void DB_HR_PULSE_RESET(uint8_t high_valid)
{	
	DB_HR_PULSE_SET((!high_valid));
}


void GPIO_HR_PULSE_Init(void)
{
    stc_gpio_cfg_t stcGpioCfg;
    
    ///< ��GPIO����ʱ���ſ�
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 
    
    ///< �˿ڷ�������->���(�������������ϣ����룩���ò���һ��)
    stcGpioCfg.enDir = GpioDirOut;
    ///< �˿�����������->����
    stcGpioCfg.enPu = GpioPuDisable;
    stcGpioCfg.enPd = GpioPdDisable;
//    stcGpioCfg.enOD = GpioOdEnable;
    Gpio_ClrIO(DB_HR_PULSE_Port, DB_HR_PULSE_Pin);
    Gpio_Init(DB_HR_PULSE_Port, DB_HR_PULSE_Pin, &stcGpioCfg);
    
}

uint8_t GPIO_PULSE_ENABLE_Init(void)
{
    stc_gpio_cfg_t stcGpioCfg;
    
   
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 
    
    stcGpioCfg.enDir = GpioDirOut;
	
    stcGpioCfg.enPu = GpioPuDisable;
    stcGpioCfg.enPd = GpioPdDisable;
//    stcGpioCfg.enOD = GpioOdEnable;
    Gpio_ClrIO(DB_PULSE_ENABLE_Port, DB_PULSE_ENABLE_Pin);
    Gpio_Init(DB_PULSE_ENABLE_Port, DB_PULSE_ENABLE_Pin, &stcGpioCfg);
	
    return (FALSE == Gpio_GetInputIO(DB_PULSE_ENABLE_Port, DB_PULSE_ENABLE_Pin));
}





///< ADC �����˿ڳ�ʼ��
void App_AdcPortInit(void)
{    
    ///< ���� GPIO����ʱ��
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    Gpio_SetAnalogMode(DB_AOUT_Port, DB_AOUT_Pin);        //PA00 (AIN0)
}


///< ADCģ�� ��ʼ��
void App_AdcInit(void)
{
    stc_adc_cfg_t              stcAdcCfg;

    DDL_ZERO_STRUCT(stcAdcCfg);
    
    ///< ����ADC/BGR ����ʱ��
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE); 
    
    Bgr_BgrEnable();        ///< ����BGR
    
    ///< ADC ��ʼ������
    stcAdcCfg.enAdcMode         = AdcSglMode;               ///<����ģʽ-����
    stcAdcCfg.enAdcClkDiv       = AdcMskClkDiv8;            ///<������Ƶ-8
    stcAdcCfg.enAdcSampCycleSel = AdcMskSampCycle12Clk;     ///<����������-12
    stcAdcCfg.enAdcRefVolSel    = AdcMskRefVolSelInBgr2p5;  ///<�ο���ѹѡ��-�ڲ�2.5V
    stcAdcCfg.enAdcOpBuf        = AdcMskBufDisable;         ///<OP BUF����-��
    stcAdcCfg.enInRef           = AdcMskInRefEnable;        ///<�ڲ��ο���ѹʹ��-��
    stcAdcCfg.enAdcAlign        = AdcAlignRight;            ///<ת��������뷽ʽ-��
    Adc_Init(&stcAdcCfg);
}

///< ADC ����ɨ��ģʽ ����
void App_AdcSglCfg(void)
{
    ///< ADC ����ͨ������
    Adc_CfgSglChannel(AdcExInputCH17);
    
    ///< ADC �ж�ʹ��
    Adc_EnableIrq();
    EnableNvic(ADC_IRQn, IrqLevel3, TRUE);
    
    ///< ��������ת������
//    Adc_SGL_Start();   

}



//ʱ�����ó�ʼ��
void App_ClockCfg(void)
{
    en_flash_waitcycle_t      enWaitCycle;
    stc_sysctrl_pll_cfg_t     stcPLLCfg;
    
    //�ṹ���ʼ������
    DDL_ZERO_STRUCT(stcPLLCfg);
    
    enWaitCycle = FlashWaitCycle1;
    Flash_WaitCycle(enWaitCycle);
    
    stcPLLCfg.enInFreq    = SysctrlPllInFreq4_6MHz;     //RCH 4MHz
    stcPLLCfg.enOutFreq   = SysctrlPllOutFreq36_48MHz;  //PLL ���48MHz
    stcPLLCfg.enPllClkSrc = SysctrlPllRch;              //����ʱ��Դѡ��RCH
    stcPLLCfg.enPllMul    = SysctrlPllMul12;            //4MHz x 12 = 48MHz
    Sysctrl_SetPLLFreq(&stcPLLCfg);
    Sysctrl_SetPLLStableTime(SysctrlPllStableCycle16384);
    Sysctrl_ClkSourceEnable(SysctrlClkPLL, TRUE);
    
    Sysctrl_SysClkSwitch(SysctrlClkPLL);  ///< ʱ���л�
}

//�˿����ó�ʼ��
void App_LEDPortCfg(void)
{
    stc_gpio_cfg_t         stcLEDPortCfg;
    
    DDL_ZERO_STRUCT(stcLEDPortCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //GPIO ����ʱ��ʹ��
    
    Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, FALSE);
    stcLEDPortCfg.enDir  = GpioDirOut;
    Gpio_Init(STK_LED_PORT, STK_LED_PIN, &stcLEDPortCfg);   //PD14����ΪLED ��������
}

//Timer0���ó�ʼ��
void App_Timer0Cfg(uint16_t u16Period)
{
    uint16_t                  u16ArrValue;
    uint16_t                  u16CntValue;
    stc_bt_mode0_cfg_t     stcBtBaseCfg;
    
    DDL_ZERO_STRUCT(stcBtBaseCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralBaseTim, TRUE); //Base Timer����ʱ��ʹ��
    
    stcBtBaseCfg.enWorkMode = BtWorkMode0;                  //��ʱ��ģʽ
    stcBtBaseCfg.enCT       = BtTimer;                      //��ʱ�����ܣ�����ʱ��Ϊ�ڲ�PCLK
    stcBtBaseCfg.enPRS      = BtPCLKDiv2;                 //PCLK/64
    stcBtBaseCfg.enCntMode  = Bt16bitArrMode;               //�Զ�����16λ������/��ʱ��
    stcBtBaseCfg.bEnTog     = FALSE;
    stcBtBaseCfg.bEnGate    = FALSE;
    stcBtBaseCfg.enGateP    = BtGatePositive;
    Bt_Mode0_Init(TIM0, &stcBtBaseCfg);                     //TIM0 ��ģʽ0���ܳ�ʼ��
    
    u16ArrValue = 0x10000 - u16Period;
    Bt_M0_ARRSet(TIM0, u16ArrValue);                        //��������ֵ(ARR = 0x10000 - ����)
    
    u16CntValue = 0x10000 - u16Period;
    Bt_M0_Cnt16Set(TIM0, u16CntValue);                      //���ü�����ֵ
    
    Bt_ClearIntFlag(TIM0,BtUevIrq);                         //���жϱ�־   
    Bt_Mode0_EnableIrq(TIM0);                               //ʹ��TIM0�ж�(ģʽ0ʱֻ��һ���ж�)
    EnableNvic(TIM0_IRQn, IrqLevel3, TRUE);                 //TIM0�ж�ʹ��
}




//����ģ������
void App_UartCfg(void)
{
	
	  stc_gpio_cfg_t stcGpioCfg;
    
    DDL_ZERO_STRUCT(stcGpioCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE); //GPIO����ģ��ʱ��ʹ��
    
    stcGpioCfg.enDir = GpioDirOut;
    Gpio_Init(GpioPortA,GpioPin9,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortA,GpioPin9,GpioAf1); //����PA09 ΪUART0 TX
    stcGpioCfg.enDir = GpioDirIn;
    Gpio_Init(GpioPortA,GpioPin10,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortA,GpioPin10,GpioAf1);//����PA10 ΪUART0 RX
	
	
    stc_uart_cfg_t  stcCfg;
    stc_uart_multimode_t stcMulti;
    stc_uart_baud_t stcBaud;

    DDL_ZERO_STRUCT(stcCfg);
    DDL_ZERO_STRUCT(stcMulti);
    DDL_ZERO_STRUCT(stcBaud);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralUart0,TRUE);//UART0����ģ��ʱ��ʹ��
    
    stcCfg.enRunMode = UartMskMode3;     //ģʽ3
    stcCfg.enStopBit = UartMsk1bit;      //1λֹͣλ
    stcCfg.enMmdorCk = UartMskEven;      //żУ��
    stcCfg.stcBaud.u32Baud = 115200;       //������115200
    stcCfg.stcBaud.enClkDiv = UartMsk8Or16Div;         //ͨ��������Ƶ����
    stcCfg.stcBaud.u32Pclk = Sysctrl_GetPClkFreq();    //�������ʱ�ӣ�PCLK��Ƶ��ֵ
    Uart_Init(M0P_UART0, &stcCfg);       //���ڳ�ʼ��

    Uart_ClrStatus(M0P_UART0,UartRC);    //���������
    Uart_ClrStatus(M0P_UART0,UartTC);    //�巢������
    Uart_EnableIrq(M0P_UART0,UartRxIrq); //ʹ�ܴ��ڽ����ж�
    Uart_EnableIrq(M0P_UART0,UartTxIrq); //ʹ�ܴ��ڷ����ж�

}




