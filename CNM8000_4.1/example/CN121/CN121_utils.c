
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
	    ///< 打开GPIO外设时钟门控
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    ///< 端口方向配置->输入
    stcGpioCfg.enDir = GpioDirIn;
    ///< 端口驱动能力配置->高驱动能力
    stcGpioCfg.enDrv = GpioDrvL;
    ///< 端口上下拉配置->无
    stcGpioCfg.enPu = GpioPuDisable;
    stcGpioCfg.enPd = GpioPdDisable;
    ///< 端口开漏输出配置->开漏输出关闭
    stcGpioCfg.enOD = GpioOdDisable;
    ///< 端口输入/输出值寄存器总线控制模式配置->AHB
    stcGpioCfg.enCtrlMode = GpioAHB;
		
    ///< GPIO IO USER KEY初始化
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
    
    ///< 打开GPIO外设时钟门控
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 
    
    ///< 端口方向配置->输出(其它参数与以上（输入）配置参数一致)
    stcGpioCfg.enDir = GpioDirOut;
    ///< 端口上下拉配置->下拉
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





///< ADC 采样端口初始化
void App_AdcPortInit(void)
{    
    ///< 开启 GPIO外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    Gpio_SetAnalogMode(DB_AOUT_Port, DB_AOUT_Pin);        //PA00 (AIN0)
}


///< ADC模块 初始化
void App_AdcInit(void)
{
    stc_adc_cfg_t              stcAdcCfg;

    DDL_ZERO_STRUCT(stcAdcCfg);
    
    ///< 开启ADC/BGR 外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE); 
    
    Bgr_BgrEnable();        ///< 开启BGR
    
    ///< ADC 初始化配置
    stcAdcCfg.enAdcMode         = AdcSglMode;               ///<采样模式-单次
    stcAdcCfg.enAdcClkDiv       = AdcMskClkDiv8;            ///<采样分频-8
    stcAdcCfg.enAdcSampCycleSel = AdcMskSampCycle12Clk;     ///<采样周期数-12
    stcAdcCfg.enAdcRefVolSel    = AdcMskRefVolSelInBgr2p5;  ///<参考电压选择-内部2.5V
    stcAdcCfg.enAdcOpBuf        = AdcMskBufDisable;         ///<OP BUF配置-关
    stcAdcCfg.enInRef           = AdcMskInRefEnable;        ///<内部参考电压使能-开
    stcAdcCfg.enAdcAlign        = AdcAlignRight;            ///<转换结果对齐方式-右
    Adc_Init(&stcAdcCfg);
}

///< ADC 单次扫描模式 配置
void App_AdcSglCfg(void)
{
    ///< ADC 采样通道配置
    Adc_CfgSglChannel(AdcExInputCH17);
    
    ///< ADC 中断使能
    Adc_EnableIrq();
    EnableNvic(ADC_IRQn, IrqLevel3, TRUE);
    
    ///< 启动单次转换采样
//    Adc_SGL_Start();   

}



//时钟配置初始化
void App_ClockCfg(void)
{
    en_flash_waitcycle_t      enWaitCycle;
    stc_sysctrl_pll_cfg_t     stcPLLCfg;
    
    //结构体初始化清零
    DDL_ZERO_STRUCT(stcPLLCfg);
    
    enWaitCycle = FlashWaitCycle1;
    Flash_WaitCycle(enWaitCycle);
    
    stcPLLCfg.enInFreq    = SysctrlPllInFreq4_6MHz;     //RCH 4MHz
    stcPLLCfg.enOutFreq   = SysctrlPllOutFreq36_48MHz;  //PLL 输出48MHz
    stcPLLCfg.enPllClkSrc = SysctrlPllRch;              //输入时钟源选择RCH
    stcPLLCfg.enPllMul    = SysctrlPllMul12;            //4MHz x 12 = 48MHz
    Sysctrl_SetPLLFreq(&stcPLLCfg);
    Sysctrl_SetPLLStableTime(SysctrlPllStableCycle16384);
    Sysctrl_ClkSourceEnable(SysctrlClkPLL, TRUE);
    
    Sysctrl_SysClkSwitch(SysctrlClkPLL);  ///< 时钟切换
}

//端口配置初始化
void App_LEDPortCfg(void)
{
    stc_gpio_cfg_t         stcLEDPortCfg;
    
    DDL_ZERO_STRUCT(stcLEDPortCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //GPIO 外设时钟使能
    
    Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, FALSE);
    stcLEDPortCfg.enDir  = GpioDirOut;
    Gpio_Init(STK_LED_PORT, STK_LED_PIN, &stcLEDPortCfg);   //PD14设置为LED 控制引脚
}

//Timer0配置初始化
void App_Timer0Cfg(uint16_t u16Period)
{
    uint16_t                  u16ArrValue;
    uint16_t                  u16CntValue;
    stc_bt_mode0_cfg_t     stcBtBaseCfg;
    
    DDL_ZERO_STRUCT(stcBtBaseCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralBaseTim, TRUE); //Base Timer外设时钟使能
    
    stcBtBaseCfg.enWorkMode = BtWorkMode0;                  //定时器模式
    stcBtBaseCfg.enCT       = BtTimer;                      //定时器功能，计数时钟为内部PCLK
    stcBtBaseCfg.enPRS      = BtPCLKDiv2;                 //PCLK/64
    stcBtBaseCfg.enCntMode  = Bt16bitArrMode;               //自动重载16位计数器/定时器
    stcBtBaseCfg.bEnTog     = FALSE;
    stcBtBaseCfg.bEnGate    = FALSE;
    stcBtBaseCfg.enGateP    = BtGatePositive;
    Bt_Mode0_Init(TIM0, &stcBtBaseCfg);                     //TIM0 的模式0功能初始化
    
    u16ArrValue = 0x10000 - u16Period;
    Bt_M0_ARRSet(TIM0, u16ArrValue);                        //设置重载值(ARR = 0x10000 - 周期)
    
    u16CntValue = 0x10000 - u16Period;
    Bt_M0_Cnt16Set(TIM0, u16CntValue);                      //设置计数初值
    
    Bt_ClearIntFlag(TIM0,BtUevIrq);                         //清中断标志   
    Bt_Mode0_EnableIrq(TIM0);                               //使能TIM0中断(模式0时只有一个中断)
    EnableNvic(TIM0_IRQn, IrqLevel3, TRUE);                 //TIM0中断使能
}




//串口模块配置
void App_UartCfg(void)
{
	
	  stc_gpio_cfg_t stcGpioCfg;
    
    DDL_ZERO_STRUCT(stcGpioCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE); //GPIO外设模块时钟使能
    
    stcGpioCfg.enDir = GpioDirOut;
    Gpio_Init(GpioPortA,GpioPin9,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortA,GpioPin9,GpioAf1); //配置PA09 为UART0 TX
    stcGpioCfg.enDir = GpioDirIn;
    Gpio_Init(GpioPortA,GpioPin10,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortA,GpioPin10,GpioAf1);//配置PA10 为UART0 RX
	
	
    stc_uart_cfg_t  stcCfg;
    stc_uart_multimode_t stcMulti;
    stc_uart_baud_t stcBaud;

    DDL_ZERO_STRUCT(stcCfg);
    DDL_ZERO_STRUCT(stcMulti);
    DDL_ZERO_STRUCT(stcBaud);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralUart0,TRUE);//UART0外设模块时钟使能
    
    stcCfg.enRunMode = UartMskMode3;     //模式3
    stcCfg.enStopBit = UartMsk1bit;      //1位停止位
    stcCfg.enMmdorCk = UartMskEven;      //偶校验
    stcCfg.stcBaud.u32Baud = 115200;       //波特率115200
    stcCfg.stcBaud.enClkDiv = UartMsk8Or16Div;         //通道采样分频配置
    stcCfg.stcBaud.u32Pclk = Sysctrl_GetPClkFreq();    //获得外设时钟（PCLK）频率值
    Uart_Init(M0P_UART0, &stcCfg);       //串口初始化

    Uart_ClrStatus(M0P_UART0,UartRC);    //清接收请求
    Uart_ClrStatus(M0P_UART0,UartTC);    //清发送请求
    Uart_EnableIrq(M0P_UART0,UartRxIrq); //使能串口接收中断
    Uart_EnableIrq(M0P_UART0,UartTxIrq); //使能串口发送中断

}




