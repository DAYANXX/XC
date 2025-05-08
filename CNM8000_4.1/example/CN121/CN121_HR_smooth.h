

#ifndef __CN121_HR_SMOOTH_H_
#define __CN121_HR_SMOOTH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


//#define MAX_SUM_RR       10000    // 11s * 1000ms
uint16_t hr_mean_filt_rr_interval(uint16_t rr_interval, int init);


#define HR_MEAN_FILT_NUM 300
uint16_t hr_mean_filt_hr_input(uint16_t hr_input, int init);




#define SAMPLERATE 250
#define WINDOWSSIZE SAMPLERATE
#define VARTHRESHOLD 3000

int detNoise(int16_t ecg);

uint8_t det_LeadOn(int16_t ecg);

#ifdef __cplusplus
}
#endif


#endif /* USER_SYS_UTILS_H_ */
