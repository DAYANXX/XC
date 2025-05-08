

#ifndef __CN121_ECG_Filter_H_
#define __CN121_ECG_Filter_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <math.h>


float CN121_ecg_filter(float ecg, uint8_t standard_signal, uint8_t init);


float epcl_filter(float x, uint16_t fs);//h

float median_filter_simple(float x);

#ifdef __cplusplus
}
#endif


#endif /* USER_SYS_UTILS_H_ */
