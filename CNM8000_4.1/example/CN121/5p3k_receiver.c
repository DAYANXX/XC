
#include "5p3k_receiver.h"



//Timer3 Port�˿�����
void App_Timer3PortCfg(void)
{
    stc_gpio_cfg_t            stcTIM3Port;
    
    //�ṹ���ʼ������
    DDL_ZERO_STRUCT(stcTIM3Port);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //�˿�����ʱ��ʹ��
    
    stcTIM3Port.enDir  = GpioDirIn;
    //PA08����ΪTIM3_CH0A
    Gpio_Init(GpioPortA, GpioPin8, &stcTIM3Port);
    Gpio_SetAfMode(GpioPortA,GpioPin8,GpioAf2);
    
    //PA07����ΪTIM3_CH0B
    //Gpio_Init(GpioPortA, GpioPin7, &stcTIM3Port);
    //Gpio_SetAfMode(GpioPortA,GpioPin7,GpioAf4);
}

//Timer3 ����
void App_Timer3Cfg(void)
{
    uint16_t                     u16CntValue;

    stc_tim3_mode1_cfg_t      stcTim3BaseCfg;
    stc_tim3_pwc_input_cfg_t  stcTim3PwcInCfg;

    //�ṹ���ʼ������
    DDL_ZERO_STRUCT(stcTim3BaseCfg);

    Sysctrl_SetPeripheralGate(SysctrlPeripheralTim3, TRUE); //Timer3 ����ʱ��ʹ��
    
    stcTim3BaseCfg.enWorkMode = Tim3WorkMode1;                //��ʱ��ģʽ
    stcTim3BaseCfg.enCT       = Tim3Timer;                    //��ʱ�����ܣ�����ʱ��Ϊ�ڲ�PCLK
    stcTim3BaseCfg.enPRS      = Tim3PCLKDiv1;                 //PCLK
    stcTim3BaseCfg.enOneShot  = Tim3PwcCycleDetect;           //PWCѭ�����
    
    Tim3_Mode1_Init(&stcTim3BaseCfg);                         //TIM3 ��ģʽ1���ܳ�ʼ��
    
    stcTim3PwcInCfg.enTsSel  = Tim3Ts6IAFP;                   //PWC����ѡ�� CHA
    stcTim3PwcInCfg.enIA0Sel = Tim3IA0Input;                  //CHAѡ��IA0
    stcTim3PwcInCfg.enFltIA0 = Tim3FltPCLKDiv16Cnt3;          //PCLK/16 3��������Ч
    //stcTim3PwcInCfg.enIB0Sel = Tim3IB0Input;                //CHBѡ��IB0
    //stcTim3PwcInCfg.enFltIB0 = Tim3FltPCLKDiv16Cnt3;        //PCLK/16 3��������Ч
    Tim3_M1_Input_Cfg(&stcTim3PwcInCfg);                      //PWC��������
    
    M0P_TIM3_MODE23->CRCH0_f.CSA = 1;                         //CHA����Ϊ����ģʽ
    
    Tim3_M1_PWC_Edge_Sel(Tim3PwcRiseToRise);                  //�����ص��������������
    
    u16CntValue = 0;
    Tim3_M1_Cnt16Set(u16CntValue);                            //���ü�����ֵ
    
    Tim3_ClearIntFlag(Tim3UevIrq);                            //��Uev�жϱ�־
    Tim3_ClearIntFlag(Tim3CA0Irq);                            //�岶׽�жϱ�־
    Tim3_Mode1_EnableIrq(Tim3UevIrq);                         //ʹ��TIM3����ж�
    Tim3_Mode1_EnableIrq(Tim3CA0Irq);                         //ʹ��TIM3�����ж�
    EnableNvic(TIM3_IRQn, IrqLevel3, TRUE);                   //TIM3�ж�ʹ��
}
