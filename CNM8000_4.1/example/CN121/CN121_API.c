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
//Release Date: 2023/8/4
//Version: VB.5
//Copyright(C) CyzurTech (Shanghai) Co. Ltd. 2020-
//All rights reserved
//hzs   40hz改为20hz
#include "CN121_API.h"
#include "CN121_qrs_Det.h"
#include "CN121_ECG_Filter.h"
#include "CN121_HR_smooth.h"

#define FIFO_Depth   64
#define SAMPLE_RATE  250


typedef struct
{
    float buf[FIFO_Depth];
    uint16_t  pos_write;
    uint16_t  pos_read;
    uint16_t  fifolength;
    uint8_t  full;
} FIFO_TypeDef;

uint16_t gain = 60;

uint16_t noise_flag = 60;
volatile float adc_vol = 0;

static uint8_t autofr_force = 0;
static uint8_t config_reg = 0;
static uint8_t ch_set_reg = 0;
static uint16_t autofr_trig_cnt = 0;
static uint16_t autofr_keep_cnt = 0;
static uint16_t leadon_judge_cnt = 0;
uint8_t lod_status = LEAD_OFF;
uint8_t standard_signal = 0;

FIFO_TypeDef fifo = {0};


static float  VREF_T = 3.3f;
static float  VCM_T  = 0.9f;
static uint32_t resolution_tmp = 1 << 10;

static float  VH_LIMIT = 1.7f;
static float  VL_LIMIT = 	0.1f;

#define GAIN_Mask       0x0f
#define GAIN_60         0x00
#define GAIN_120        0x01
#define GAIN_180        0x02
#define GAIN_240        0x03
#define GAIN_300        0x04
#define GAIN_360        0x05
#define GAIN_420        0x06
#define GAIN_480        0x07
#define GAIN_540        0x08
#define GAIN_600        0x09
#define GAIN_660        0x0a
#define GAIN_720        0x0b
#define GAIN_780        0x0c
#define GAIN_840        0x0d
#define GAIN_900        0x0e
#define GAIN_960        0x0f

#define LP_Mask         0x30
#define LP_40           0x00
#define LP_55           0x10
#define LP_150          0x20
#define LP_300          0x30

#define HP_Mask         0x40
#define HP_4            0x00
#define HP_0p05         0x40

#define FR_Mask         0x80
#define FR_on           0x80

#define PWR_Mask        0x40
#define PWR_off         0x40

#define STANDBY_Mask    0x20
#define STANDBY_on      0x20

#define OP_RREG         0x20
#define OP_WREG         0x40

#define ADDR_CONFIG     0x00
#define ADDR_CH_SET     0x01



//#define AUTOFR_TRIG_WIN     100  //AutoFR Trigger Window 100ms
#define AUTOFR_TRIG_LIMIT   50   //ADC_UPDATE_FREQ*AUTOFR_TRIG_WIN/1000
//#define AUTOFR_KEEP_TIME    100  //AutoFR Keep Time 100ms
#define AUTOFR_KEEP_LIMIT   50   //ADC_UPDATE_FREQ*AUTOFR_KEEP_TIME/1000
//#define LEADON_JUDGE_WIN    500  //Leadoff Judge Window 500ms
#define LEADON_JUDGE_LIMIT  50   //LOD_UPDATE_FREQ*LEADOFF_JUDGE_WIN/1000


void CN121_Parameter_Init(uint8_t Use_For_Standard, uint8_t ADC_RESOLUTION_12, float VREF, float VVCM)
{
	VREF_T = VREF;
	VCM_T  = VVCM;

//	VH_LIMIT =  VCM_T * 10 / 6;     //VCM_T * 2 * 5 / 6;
//	VL_LIMIT =  VCM_T / 3;  		//VCM_T * 2 / 6;

	if(ADC_RESOLUTION_12 == 1)
	{
		resolution_tmp = 1 << 12;
	}
	standard_signal = Use_For_Standard;
	CN121_ecg_filter(0.0f, standard_signal, 1);
}


void SPI_ReadWrite(uint8_t* TxDataSeq,uint8_t* RxDataSeq,uint8_t SeqLen)
{
	uint8_t hw_spi = CN1xx_SPI_ReadWrite(TxDataSeq, RxDataSeq, SeqLen);
	if(hw_spi)
	{
		return;
	}

	uint8_t i = 0, j = 0, TxData = 0, RxData = 0;

	DB_CSB_RESET();
	CN1xx_delay_us(10);

	i = 0;
	while(i < SeqLen)
	{
		j = 0;
		TxData = TxDataSeq[i];
		while(j < 8)
		{
			DB_SCLK_SET();
			if(TxData & 0x80)
			{
				DB_MOSI_SET();
			}
			else
			{
				DB_MOSI_RESET();
			}
			TxData = TxData << 1;
			CN1xx_delay_us(2);
			DB_SCLK_RESET();
			RxData = RxData << 1;
			if(DB_MISO_HIGH())
			{
				RxData = RxData | 0x01;
			}
			else
			{
				RxData = RxData & 0xFE;
			}
			CN1xx_delay_us(2);
			j++;
		}
		RxDataSeq[i] = RxData;
		i++;
	}

	CN1xx_delay_us(10);
	DB_CSB_SET();
	CN1xx_delay_us(10);
}



float ADC2Voltage(uint16_t adc)
{
	return (float)adc/ resolution_tmp *VREF_T;
}


uint8_t CN121_PowerOn()
{
    uint8_t opcode1 = OP_WREG | ADDR_CONFIG;
    uint8_t opcode2 = 0x00;
    uint8_t reg_data = config_reg;
    reg_data = reg_data & (~PWR_Mask);
    reg_data = reg_data & (~PWR_off);
    uint8_t TxDataSeq[3] = {opcode1, opcode2, reg_data};
    uint8_t RxDataSeq[3] = {0};

    SPI_ReadWrite(TxDataSeq, RxDataSeq, 3);

    opcode1 = OP_RREG | ADDR_CONFIG;
    opcode2 = 0x00;
    TxDataSeq[0] = opcode1;
    TxDataSeq[1] = opcode2;
    TxDataSeq[2] = 0xFF;

    SPI_ReadWrite(TxDataSeq, RxDataSeq, 3);

    if (RxDataSeq[2] == reg_data)
    {
        config_reg = reg_data;
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t CN121_PowerOff()
{
    uint8_t opcode1 = OP_WREG | ADDR_CONFIG;
    uint8_t opcode2 = 0x00;
    uint8_t reg_data = config_reg;
    reg_data = reg_data & (~PWR_Mask);
    reg_data = reg_data | PWR_off;
    uint8_t TxDataSeq[3] = {opcode1, opcode2, reg_data};
    uint8_t RxDataSeq[3] = {0};

    SPI_ReadWrite(TxDataSeq, RxDataSeq, 3);

    opcode1 = OP_RREG | ADDR_CONFIG;
    opcode2 = 0x00;
    TxDataSeq[0] = opcode1;
    TxDataSeq[1] = opcode2;
    TxDataSeq[2] = 0xFF;

    SPI_ReadWrite(TxDataSeq, RxDataSeq, 3);

    if (RxDataSeq[2] == reg_data)
    {
        config_reg = reg_data;
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t CN121_StandbyOn()
{
    uint8_t opcode1 = OP_WREG | ADDR_CONFIG;
    uint8_t opcode2 = 0x00;
    uint8_t reg_data = config_reg;
    reg_data = reg_data & (~STANDBY_Mask);
    reg_data = reg_data | STANDBY_on;
    uint8_t TxDataSeq[3] = {opcode1, opcode2, reg_data};
    uint8_t RxDataSeq[3] = {0};

    SPI_ReadWrite(TxDataSeq, RxDataSeq, 3);

    opcode1 = OP_RREG | ADDR_CONFIG;
    opcode2 = 0x00;
    TxDataSeq[0] = opcode1;
    TxDataSeq[1] = opcode2;
    TxDataSeq[2] = 0xFF;

    SPI_ReadWrite(TxDataSeq, RxDataSeq, 3);

    if (RxDataSeq[2] == reg_data)
    {
        config_reg = reg_data;
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t CN121_StandbyOff()
{
    uint8_t opcode1 = OP_WREG | ADDR_CONFIG;
    uint8_t opcode2 = 0x00;
    uint8_t reg_data = config_reg;
    reg_data = reg_data & (~STANDBY_Mask);
    reg_data = reg_data & (~STANDBY_on);
    uint8_t TxDataSeq[3] = {opcode1, opcode2, reg_data};
    uint8_t RxDataSeq[3] = {0};

    SPI_ReadWrite(TxDataSeq, RxDataSeq, 3);

    opcode1 = OP_RREG | ADDR_CONFIG;
    opcode2 = 0x00;
    TxDataSeq[0] = opcode1;
    TxDataSeq[1] = opcode2;
    TxDataSeq[2] = 0xFF;

    SPI_ReadWrite(TxDataSeq, RxDataSeq, 3);

    if (RxDataSeq[2] == reg_data)
    {
        config_reg = reg_data;
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t CN121_FR_On()
{
    uint8_t opcode1 = OP_WREG | ADDR_CH_SET;
    uint8_t opcode2 = 0x00;
    uint8_t reg_data = ch_set_reg;
    reg_data = reg_data & (~FR_Mask);
    reg_data = reg_data | FR_on;
    uint8_t TxDataSeq[3] = {opcode1, opcode2, reg_data};
    uint8_t RxDataSeq[3] = {0};

    SPI_ReadWrite(TxDataSeq, RxDataSeq, 3);

    opcode1 = OP_RREG | ADDR_CH_SET;
    opcode2 = 0x00;
    TxDataSeq[0] = opcode1;
    TxDataSeq[1] = opcode2;
    TxDataSeq[2] = 0xFF;

    SPI_ReadWrite(TxDataSeq, RxDataSeq, 3);

    if (RxDataSeq[2] == reg_data)
    {
        ch_set_reg = reg_data;
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t CN121_FR_Off()
{
    uint8_t opcode1 = OP_WREG | ADDR_CH_SET;
    uint8_t opcode2 = 0x00;
    uint8_t reg_data = ch_set_reg;
    reg_data = reg_data & (~FR_Mask);
    reg_data = reg_data & (~FR_on);
    uint8_t TxDataSeq[3] = {opcode1, opcode2, reg_data};
    uint8_t RxDataSeq[3] = {0};

    SPI_ReadWrite(TxDataSeq, RxDataSeq, 3);

    opcode1 = OP_RREG | ADDR_CH_SET;
    opcode2 = 0x00;
    TxDataSeq[0] = opcode1;
    TxDataSeq[1] = opcode2;
    TxDataSeq[2] = 0xFF;

    SPI_ReadWrite(TxDataSeq, RxDataSeq, 3);

    if (RxDataSeq[2] == reg_data)
    {
        ch_set_reg = reg_data;
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t CN121_Set_Gain(uint8_t gain)
{
    uint8_t opcode1 = OP_WREG | ADDR_CH_SET;
    uint8_t opcode2 = 0x00;
    uint8_t reg_data = ch_set_reg;
    reg_data = reg_data & (~GAIN_Mask);
    reg_data = reg_data | gain;
    uint8_t TxDataSeq[3] = {opcode1, opcode2, reg_data};
    uint8_t RxDataSeq[3] = {0};

    SPI_ReadWrite(TxDataSeq, RxDataSeq, 3);

    opcode1 = OP_RREG | ADDR_CH_SET;
    opcode2 = 0x00;
    TxDataSeq[0] = opcode1;
    TxDataSeq[1] = opcode2;
    TxDataSeq[2] = 0xFF;

    SPI_ReadWrite(TxDataSeq, RxDataSeq, 3);

    if (RxDataSeq[2] == reg_data)
    {
        ch_set_reg = reg_data;
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t CN121_Set_Bandwidth(uint8_t hp, uint8_t lp)
{
    uint8_t opcode1 = OP_WREG | ADDR_CH_SET;
    uint8_t opcode2 = 0x00;
    uint8_t reg_data = ch_set_reg;
    reg_data = reg_data & (~HP_Mask);
    reg_data = reg_data | hp;
    reg_data = reg_data & (~LP_Mask);
    reg_data = reg_data | lp;
    uint8_t TxDataSeq[3] = {opcode1, opcode2, reg_data};
    uint8_t RxDataSeq[3] = {0};

    SPI_ReadWrite(TxDataSeq, RxDataSeq, 3);

    opcode1 = OP_RREG | ADDR_CH_SET;
    opcode2 = 0x00;
    TxDataSeq[0] = opcode1;
    TxDataSeq[1] = opcode2;
    TxDataSeq[2] = 0xFF;

    SPI_ReadWrite(TxDataSeq, RxDataSeq, 3);

    if (RxDataSeq[2] == reg_data)
    {
        ch_set_reg = reg_data;
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t CN121_AutoFR_Trig(float adc_vol)
{
    if (adc_vol < VL_LIMIT || adc_vol > VH_LIMIT)
    {
        autofr_trig_cnt++;
    }
    else
    {
        autofr_trig_cnt = 0;
    }
    if (autofr_trig_cnt > AUTOFR_TRIG_LIMIT)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t CN121_AutoFR(uint8_t force, float adc_vol_det)
{
    if (autofr_keep_cnt > 0)
    {
        autofr_keep_cnt++;
    }
    if (autofr_keep_cnt > AUTOFR_KEEP_LIMIT)
    {
        CN121_FR_Off();
        autofr_keep_cnt = 0;
        autofr_force = 0;
        return 2;
    }
	if(force == 1)
	{
		autofr_force = 1;
        CN121_FR_On();
		adc_vol = adc_vol_det;
        autofr_keep_cnt++;
        return 1;
	}
	else if (autofr_force == 0 && CN121_AutoFR_Trig(adc_vol_det))
    {
        CN121_FR_On();
        autofr_keep_cnt++;
        return 1;
    }
    return 0;
}

void CN121_LOD_Detect(void)
{
    if (DB_LOD1_LOW())
    {
        leadon_judge_cnt++;
        if (leadon_judge_cnt > LEADON_JUDGE_LIMIT)
        {
            lod_status = LEAD_ON;
            leadon_judge_cnt = 0;
        }
    }
    else
    {
        leadon_judge_cnt = 0;
        lod_status = LEAD_OFF;
    }
}

//void CN121_LOD_Detect(void)
//{
//    if (DB_LOD1_HIGH())
//    {
//        leadon_judge_cnt++;
//        if (leadon_judge_cnt > LEADON_JUDGE_LIMIT)
//        {
//            lod_status = LEAD_OFF;
//            leadon_judge_cnt = 0;
//        }
//    }
//    else
//    {
//        leadon_judge_cnt = 0;
//        lod_status = LEAD_ON;
//    }
//}



void CN121_Reset(void)
{
//	DB_RST_SET();
    CN1xx_delay_us(1);
//    DB_RST_RESET();

    config_reg = 0;
    ch_set_reg = 0;
}


uint8_t FIFO_Push(float val)
{
	if(fifo.full == 1)
		return 1;

    fifo.buf[fifo.pos_write++] = val;
    fifo.pos_write %= FIFO_Depth;
    fifo.fifolength++;
    if(fifo.pos_write == fifo.pos_read)
    {
    	fifo.full = 1;
    }
    return 0;
}

uint8_t FIFO_Pop(float *val)
{
	if(fifo.full == 0 && fifo.pos_read == fifo.pos_write)
		return 1;

    *val = fifo.buf[fifo.pos_read++];
    fifo.fifolength--;
    fifo.pos_read %= FIFO_Depth;
    fifo.full = 0;
    return 0;
}

void FIFO_Clear()
{
    memset(&fifo, 0, sizeof(FIFO_TypeDef));
}

uint8_t FIFO_Full()
{
	return fifo.full;
}

uint16_t FIFO_Size()
{
	return fifo.fifolength;
}

uint8_t CN121_Init(CN121_WORKMODE Work_mode)
{
	uint8_t gain_t = GAIN_120;
	uint8_t hp_t   = HP_0p05;
	uint8_t lp_t   = LP_300;

	switch (Work_mode) {
		case (Medical_MODE):
			hp_t = HP_0p05;
			gain_t = GAIN_120;
			gain   = 120;
			break;
		case (Health_MODE):
			hp_t = HP_4;
			gain_t = GAIN_480;
			gain   = 480;
			break;
		default:
			break;
	}

	CN121_Reset();
	uint8_t result = (CN121_PowerOn() && CN121_StandbyOff() && CN121_FR_Off()
			&& CN121_Set_Gain(gain_t) && CN121_Set_Bandwidth(hp_t, lp_t));
	if(result == 0)
	{
		gain = 60;
	}
	return result;
}

uint8_t CN121_Standby(void)
{
	return (CN121_PowerOn() && CN121_StandbyOn() && CN121_FR_Off());
}

void CN121_Start(void)
{
}


void CN121_LOD_ON_Init(CN121_LOD_On_MODE init_mode)
{
	FIFO_Clear();
//	CN121_AutoFR(1,0.9);
	if(init_mode == realRhythm)
	{
		QRSDet_V2_init();
	}
}

uint8_t CN121_Data_Proc(CN121_PROC cn121_proc, int16_t *ecg, int16_t *RDelay)
{
	uint8_t result = 0;
	float adc_value_real = 0;
	switch (cn121_proc) {
		case (TIMER_IT_PROC):
				CN121_LOD_Detect();
			break;
		case (ADC_IT_PROC):
			adc_vol = ADC2Voltage(*ecg);
			result = FIFO_Push(adc_vol);
			break;
		case (GET_VALUE_PROC):

				result = CN121_AutoFR(0,adc_vol);
			result = FIFO_Pop(&adc_value_real);
			if(0 == result)
			{
				float ecg_clean = CN121_ecg_filter((adc_value_real - VCM_T), standard_signal, 0);
				float ecg_norm = ecg_clean/gain*1000000;
				*ecg = ecg_norm > 0.0f ? (int16_t)(ecg_norm + 0.5f) : (int16_t)(ecg_norm - 0.5f);
			}
			if(FIFO_Full())
			{
				result = 2;
			}
			break;
		case (GET_VALUE_Rdet_PROC):
				result = CN121_AutoFR(0,adc_vol);
			result = FIFO_Pop(&adc_value_real);
			if(0 == result)
			{

				float ecg_clean = CN121_ecg_filter((adc_value_real - VCM_T), standard_signal, 0);

				float ecg_norm = ecg_clean/gain*1000000;
				
				
//				int16_t ecg_1 = ecg_norm > 0.0f ? (int16_t)(ecg_norm + 0.5f) : (int16_t)(ecg_norm - 0.5f);
//				ecg_norm = epcl_filter(ecg_1, 200);//h 是否带光滑算法
					
				*ecg = ecg_norm > 0.0f ? (int16_t)(ecg_norm + 0.5f) : (int16_t)(ecg_norm - 0.5f);
				*RDelay = QRSDet_V2(*ecg);
//				lod_status = det_LeadOn(*ecg);

			}
			if(FIFO_Full())
			{
				result = 2;
			}

			break;
		default:
			break;
	}
	return  result;
}

uint8_t CN121_Stop(void)
{
	return (CN121_PowerOff());
}


uint16_t CN121_heart_Rate(void)
{
	return get_heart_Rate();
}

