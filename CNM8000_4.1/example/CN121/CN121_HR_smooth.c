
#include "CN121_HR_smooth.h"



uint16_t hr_mean_filt_rr_interval(uint16_t rr_interval, int init)
{
    static uint16_t sum_RR = 0;
    static uint16_t hr_mean_value = 0;
    static uint16_t fastIndex = 0;
    static uint16_t slowIndex = 0;
    static uint16_t hr_filt_State[HR_MEAN_FILT_NUM] = {0};
		static uint16_t rr_num = 0;
		static uint32_t MAX_SUM_RR = 15000;
		static uint8_t over_cur_hr_cnt = 0;
		static uint32_t hr_total_cnt = 0;
		uint8_t max_over_abs = 0;

    if(init)
    {
        sum_RR = 0;
        hr_mean_value = 0;
        fastIndex = 0;
        slowIndex = 0;
				rr_num = 0;
				hr_total_cnt = 0;

        memset(hr_filt_State, 0, sizeof(hr_filt_State));
        return hr_mean_value;
    }

    if(rr_interval < 235 || rr_interval > 2010)  // bpm 30 ~250
    {
        return hr_mean_value;
    }

	

    hr_filt_State[fastIndex++] = rr_interval;
    fastIndex %= HR_MEAN_FILT_NUM;
		rr_num++;

    sum_RR += rr_interval;
		
		if(hr_mean_value < 96) {
        MAX_SUM_RR = 15000;//15000;
		}  
    else if (96 <= hr_mean_value && hr_mean_value < 115) {
        MAX_SUM_RR = 10000;//10000;
		}
    else if (115 <= hr_mean_value && hr_mean_value < 153) {
        MAX_SUM_RR = 6000;//6000;
		}
    else if (153 <= hr_mean_value && hr_mean_value < 172) {
        MAX_SUM_RR = 5000; //5000;
		}
    else if (172 <= hr_mean_value && hr_mean_value < 191) {
        MAX_SUM_RR = 4000; //4000;
		}
    else {
        MAX_SUM_RR = 3000; //3000;
		}
		
		
//		MAX_SUM_RR  =  MAX_SUM_RR *2;

    while(sum_RR > MAX_SUM_RR)
    {
				sum_RR -= hr_filt_State[slowIndex];
        slowIndex++;
        slowIndex %= HR_MEAN_FILT_NUM;
				rr_num--;
    }
		
    hr_mean_value = (60000 * rr_num / sum_RR);

    return hr_mean_value;
}


uint16_t hr_mean_filt_hr_input(uint16_t hr_input, int init)
{
    static float sum_HR = 0;
    static uint16_t hr_pnt = 0;
    static uint16_t hr_num = 0;
    static uint16_t hr_mean_value = 0;
		static uint16_t hr_filt_State[HR_MEAN_FILT_NUM] = {0};

    if(init)
    {
        sum_HR = 0;
        hr_pnt = 0;
        hr_num = 0;
//        hr_mean_value = 0;

        memset(hr_filt_State, 0, sizeof(hr_filt_State));
        return hr_mean_value;
    }

    if(hr_input > 200 || hr_input < 40)
    {
        return hr_mean_value;
    }

    sum_HR -= hr_filt_State[hr_pnt];
    sum_HR += hr_input;

    hr_filt_State[hr_pnt++] = hr_input;
    hr_pnt %= HR_MEAN_FILT_NUM;
    hr_num++;

    if(hr_num >= HR_MEAN_FILT_NUM)
    {
        hr_mean_value = sum_HR / HR_MEAN_FILT_NUM + 0.5f;
    }
    else
    {
        hr_mean_value = sum_HR / hr_pnt + 0.5f;
    }

    return hr_mean_value;
}



int calmean(int num[], int size)
{
    int result = 0;
    for(int i = 0; i < size; i++) {
        result += num[i];
    }
    result /= size;
    
    return result;
}

uint32_t calVar(int num[], int size)
{
    int mean0 = calmean(num,size);
    uint32_t result = 0;
    for(int i = 0; i < size; i++) {
        result += abs(num[i] - mean0);
    }
    result /= size;
    return result;
}


int detNoise(int16_t ecg)
{
    static int increase = 0;
    static int ecgSection[WINDOWSSIZE]  = {0};

    static int result = 0;
    ecgSection[increase++] = ecg;

    if(increase == WINDOWSSIZE) {
        uint32_t ecg_var = calVar(ecgSection, WINDOWSSIZE);
        if(ecg_var >= VARTHRESHOLD) {
            result = 1;
        }
        else {
          result = 0;
        }
        
        increase = 0;
    }

    return result;
}


#define NOISE_THE 50

uint8_t det_LeadOn(int16_t ecg) {
    static int16_t pre_ecg =0;
    static int32_t sum_diff = 0;
    static int16_t ecg_count = 0;
    static uint8_t lead_on_soft = 0;
    
		int16_t diff = ecg - pre_ecg; 
		sum_diff += abs(diff);  
		ecg_count++;
	
		if(ecg_count >= 250) {		
			int32_t noise_level = sum_diff / ecg_count; 
			if(noise_level >= NOISE_THE) 
			{
				lead_on_soft = 1;
			}
			else 
			{
				lead_on_soft = 0;
			}
			sum_diff = 0;
			ecg_count = 0;	
		}

    return lead_on_soft;
}

