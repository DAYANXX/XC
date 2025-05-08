
#ifndef __CN121_QRS_DET_H_
#define __CN121_QRS_DET_H_


#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include "CN121_HR_smooth.h"

int16_t QRSDet_V2_init(void);


int16_t QRSDet_V2(int16_t x);

uint16_t get_heart_Rate(void);


#endif /* CN121_CN121_API_H_ */
