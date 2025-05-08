
#include "5p3k_receiver.h"



//Timer3 Port端口配置
void App_Timer3PortCfg(void)
{
    stc_gpio_cfg_t            stcTIM3Port;
    
    //结构体初始化清零
    DDL_ZERO_STRUCT(stcTIM3Port);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //端口外设时钟使能
    
    stcTIM3Port.enDir  = GpioDirIn;
    //PA08设置为TIM3_CH0A
    Gpio_Init(GpioPortA, GpioPin8, &stcTIM3Port);
    Gpio_SetAfMode(GpioPortA,GpioPin8,GpioAf2);
    
    //PA07设置为TIM3_CH0B
    //Gpio_Init(GpioPortA, GpioPin7, &stcTIM3Port);
    //Gpio_SetAfMode(GpioPortA,GpioPin7,GpioAf4);
}

//Timer3 配置
void App_Timer3Cfg(void)
{
    uint16_t                     u16CntValue;

    stc_tim3_mode1_cfg_t      stcTim3BaseCfg;
    stc_tim3_pwc_input_cfg_t  stcTim3PwcInCfg;

    //结构体初始化清零
    DDL_ZERO_STRUCT(stcTim3BaseCfg);

    Sysctrl_SetPeripheralGate(SysctrlPeripheralTim3, TRUE); //Timer3 外设时钟使能
    
    stcTim3BaseCfg.enWorkMode = Tim3WorkMode1;                //定时器模式
    stcTim3BaseCfg.enCT       = Tim3Timer;                    //定时器功能，计数时钟为内部PCLK
    stcTim3BaseCfg.enPRS      = Tim3PCLKDiv1;                 //PCLK
    stcTim3BaseCfg.enOneShot  = Tim3PwcCycleDetect;           //PWC循环检测
    
    Tim3_Mode1_Init(&stcTim3BaseCfg);                         //TIM3 的模式1功能初始化
    
    stcTim3PwcInCfg.enTsSel  = Tim3Ts6IAFP;                   //PWC输入选择 CHA
    stcTim3PwcInCfg.enIA0Sel = Tim3IA0Input;                  //CHA选择IA0
    stcTim3PwcInCfg.enFltIA0 = Tim3FltPCLKDiv16Cnt3;          //PCLK/16 3个连续有效
    //stcTim3PwcInCfg.enIB0Sel = Tim3IB0Input;                //CHB选择IB0
    //stcTim3PwcInCfg.enFltIB0 = Tim3FltPCLKDiv16Cnt3;        //PCLK/16 3个连续有效
    Tim3_M1_Input_Cfg(&stcTim3PwcInCfg);                      //PWC输入设置
    
    M0P_TIM3_MODE23->CRCH0_f.CSA = 1;                         //CHA设置为捕获模式
    
    Tim3_M1_PWC_Edge_Sel(Tim3PwcRiseToRise);                  //上升沿到上升沿脉宽测量
    
    u16CntValue = 0;
    Tim3_M1_Cnt16Set(u16CntValue);                            //设置计数初值
    
    Tim3_ClearIntFlag(Tim3UevIrq);                            //清Uev中断标志
    Tim3_ClearIntFlag(Tim3CA0Irq);                            //清捕捉中断标志
    Tim3_Mode1_EnableIrq(Tim3UevIrq);                         //使能TIM3溢出中断
    Tim3_Mode1_EnableIrq(Tim3CA0Irq);                         //使能TIM3捕获中断
    EnableNvic(TIM3_IRQn, IrqLevel3, TRUE);                   //TIM3中断使能
}
