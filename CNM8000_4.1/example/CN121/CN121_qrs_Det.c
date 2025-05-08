
#include "CN121_qrs_Det.h"

volatile uint16_t heart_Rate = 0;

#define SAMPLE_RATE 250
#define DIFF_FILT_NUM_TAPS 4  // diff filter window
#define QRS_REC_LEN 10		  // QRS wave recording length
#define QRS_REC_UPDATE_SIZE 4 // Threshold update length
#define ECG_ARRAY_TIME 6	  // ECG data recording time

#define ECG_H_MAX_SIZE (ECG_ARRAY_TIME * SAMPLE_RATE)
#define ECG_M_MAX_SIZE (ECG_ARRAY_TIME * SAMPLE_RATE)

static int16_t DIFF_firCoeffs[] = {
	-2147, 21450, -21450, 2147};

static uint32_t in_index = 0;
static uint32_t MS298 = 0;
// static uint32_t MS150 = 0;

static int16_t ecg_m[ECG_M_MAX_SIZE] = {0};
static uint32_t ecg_m_pnt = 0;
static uint32_t ecg_m_siz = 0;
// static qrs_i_t  qrs_i[QRS_REC_LEN] = {0};
// static uint32_t qrs_i_pnt = 0;
static uint32_t qrs_i_num = 0;

static int16_t ecg_m_r[QRS_REC_UPDATE_SIZE] = {0};
static uint32_t ecg_m_r_pnt = 0;

static void update_thr_ecg_m(int32_t *THR_ECG_M, int16_t out_index_k_sum, int type);
static uint32_t ecg_m_find_r_point(uint32_t offset);
int16_t diff_filt(int16_t datum, int32_t init);
int16_t extreme_det(int16_t datum, int32_t init);
static int compar(const void *a, const void *b);
int16_t median(int16_t *array, uint32_t nitems);
static void RRUpdate(int RRNew);

int16_t QRSDet_V2_init()
{
	in_index = 0;
	// MS298 = 0.298 * (float)SAMPLE_RATE + 0.5;
	MS298 = 0.235f * (float)SAMPLE_RATE + 0.5f;
	//    MS150 = 0.150 * (float)SAMPLE_RATE + 0.5;

	memset(ecg_m, 0, sizeof(ecg_m));
	ecg_m_pnt = 0;
	ecg_m_siz = SAMPLE_RATE * ECG_ARRAY_TIME;
	//    memset(qrs_i, 0, sizeof(qrs_i));
	//    qrs_i_pnt = 0;
	qrs_i_num = 0;

	//    bp_filt(0, 1);
	diff_filt(0, 1);
	extreme_det(0, 1);
	hr_mean_filt_rr_interval(0, 1);

	return 1;
}

int16_t QRSDet_V2(int16_t x)
{
	static int32_t THR_ECG_M, THR_ECG_M_max;
	static uint32_t last_rec_index = 0;
	static int32_t last_rec_k_sum = 0;
	static int32_t last_r_index = 0;
	uint32_t index_discard = SAMPLE_RATE<<1;
	uint32_t index_thre = (SAMPLE_RATE << 2);
	 uint32_t index_thre_temp = (SAMPLE_RATE << 2);
//	uint32_t index_thre_temp = (uint32_t)(SAMPLE_RATE * 3);
//	uint32_t index_thre_MAX = (uint32_t)(SAMPLE_RATE * 1.5);

	int16_t sig_bp = 0;
	int16_t sig_diff = 0;
	int16_t k_sum = 0;
	uint32_t out_index = 0;
	int16_t out_index_k_sum = 0;
	uint32_t temp_index = 0;
	int16_t RDelay = 0;

	//	    memset(ppqrst, 0, sizeof(pqrst_t));

	if (0 == in_index)
	{
		THR_ECG_M = 0;
		THR_ECG_M_max = 0;

		last_rec_index = 0;
		last_rec_k_sum = 0;
		last_r_index = 0;
	}
	else if (in_index == index_thre + 1)
	{
		THR_ECG_M = THR_ECG_M_max / 2;
		last_r_index = index_thre;
	}

	sig_bp = x;

	sig_diff = diff_filt(sig_bp, 0);

	k_sum = extreme_det(sig_diff, 0);

	ecg_m[ecg_m_pnt++] = k_sum;
	ecg_m_pnt %= ecg_m_siz;

	if (in_index <= index_discard)
	{
		goto end;
	}

	if (in_index <= index_thre)
	{
		/* The threshold is calculated based on the sampling point within 1 seconds of starting sampling */
		THR_ECG_M_max = k_sum > THR_ECG_M_max ? k_sum : THR_ECG_M_max;
		goto end;
	}

//	if (qrs_i_num == 0 && (in_index - last_r_index >= index_thre_temp))
//	{
//		update_thr_ecg_m(&THR_ECG_M, 0, 1);
//	}

//	if (qrs_i_num >= 2 && (in_index - last_rec_index >= index_thre_MAX))
//	{
//		update_thr_ecg_m(&THR_ECG_M, 0, 1);
//	}

	/**/
	 if(in_index - last_r_index >= index_thre_temp)
	 {
			update_thr_ecg_m(&THR_ECG_M, 0, 1);
	 }

	if (k_sum <= THR_ECG_M)
	{
		goto end;
	}

	/* Preliminary screening for points where the slope exceeds the threshold */

	if (0 == last_rec_index)
	{
		last_rec_index = in_index;
		last_rec_k_sum = k_sum;
	}
	else if (in_index - last_rec_index < MS298)
	{
		if (k_sum > last_rec_k_sum)
		{
			last_rec_index = in_index;
			last_rec_k_sum = k_sum;
		}
	}
	else
	{
		out_index = last_rec_index;
		out_index_k_sum = last_rec_k_sum;
		last_rec_index = in_index;
		last_rec_k_sum = k_sum;
	}

	if (out_index > 0)
	{
		temp_index = in_index - ecg_m_find_r_point(in_index - out_index + 1);
	}

	if (temp_index > 0)
	{
		//			uint32_t qrs_i_pnt_pre = qrs_i_pnt > 0 ? (qrs_i_pnt - 1) : (QRS_REC_LEN - 1);
		//
		//			memset(&qrs_i[qrs_i_pnt], 0, sizeof(qrs_i_t));
		//			qrs_i[qrs_i_pnt].r = temp_index;

		if (qrs_i_num > 1)
		{
//				RRUpdate(temp_index - last_r_index);

			uint16_t rr_inter = (temp_index - last_r_index) * (1000 / SAMPLE_RATE);

			heart_Rate =  hr_mean_filt_rr_interval(rr_inter, 0);
		}

		last_r_index = temp_index;
		qrs_i_num++;

		// RDelay = in_index - temp_index + (BP_FILT_NUM_TAPS >> 1);
		RDelay = in_index - temp_index;

		/* The threshold is updated with the center value of the first 3 heartbeat data */
		//			qrs_i_pnt++;
		//			qrs_i_pnt %= QRS_REC_LEN;
		update_thr_ecg_m(&THR_ECG_M, out_index_k_sum, 0);
	}

end:
	in_index++;
	return RDelay;
}

static void update_thr_ecg_m(int32_t *THR_ECG_M, int16_t out_index_k_sum, int type)
{

	if (0 == type)
	{
		ecg_m_r[ecg_m_r_pnt++] = out_index_k_sum;
		ecg_m_r_pnt %= QRS_REC_UPDATE_SIZE;

		if (qrs_i_num >= QRS_REC_UPDATE_SIZE)
		{
			*THR_ECG_M = median(ecg_m_r, QRS_REC_UPDATE_SIZE) / 2;
			//            QRSDET_TRACE_PRINT("update 1 way THR_ECG_M=%d\n", *THR_ECG_M);
		}
		else
		{
			*THR_ECG_M = out_index_k_sum / 2;
			//            QRSDET_TRACE_PRINT("update 2 way THR_ECG_M=%d\n", *THR_ECG_M);
		}
	}
	else if (1 == type) 
	{
		*THR_ECG_M >>= 1;
		//        QRSDET_TRACE_PRINT("update 3 way THR_ECG_M=%d\n", *THR_ECG_M);
	}
}

static uint32_t ecg_m_find_r_point(uint32_t offset)
{
	uint32_t f_offset;
	uint32_t m_pnt_start, m_pnt;

	if (offset >= ecg_m_siz)
	{
		return 0;
	}
	m_pnt_start = ecg_m_pnt >= offset ? ecg_m_pnt - offset : ecg_m_pnt + ecg_m_siz - offset;
	for (f_offset = 1; f_offset < ecg_m_siz - offset; f_offset++)
	{
		m_pnt = m_pnt_start >= f_offset ? m_pnt_start - f_offset : m_pnt_start + ecg_m_siz - f_offset;
		if (ecg_m[m_pnt] != 0)
		{
			//            QRSDET_TRACE_PRINT("Find forward  point, f_offset=%d, ecg_m[%d] = %d\n", f_offset, m_pnt, ecg_m[m_pnt]);
			break;
		}
	}

	return (offset + f_offset);
}

/******************************************************************************
 *  diff_filt() implements the diff filter
 *
 *  Note that the filter delay is (DIFF_FILT_NUM_TAPS-1)/2
 ******************************************************************************/
int16_t diff_filt(int16_t datum, int32_t init)
{
	static int32_t DIFF_firState[DIFF_FILT_NUM_TAPS] = {0};
	static int32_t ptr = 0;
	int32_t i, z = 0;
	int32_t ptr1, ptr2;

	if (init)
	{
		memset(DIFF_firState, 0, sizeof(DIFF_firState));
		ptr = 0;
		return (0);
	}

	DIFF_firState[ptr] = datum;
	for (i = 0; i < DIFF_FILT_NUM_TAPS / 2; i++)
	{
		ptr1 = ptr - i;
		ptr2 = ptr - (DIFF_FILT_NUM_TAPS - 1) + i;

		if (ptr1 < 0)
		{
			ptr1 += DIFF_FILT_NUM_TAPS;
		}
		if (ptr2 < 0)
		{
			ptr2 += DIFF_FILT_NUM_TAPS;
		}

		z += (DIFF_firState[ptr1] - DIFF_firState[ptr2]) * DIFF_firCoeffs[i];
	}

	if (++ptr >= DIFF_FILT_NUM_TAPS)
	{
		ptr = 0;
	}

	return (z >> 14);
}

/******************************************************************************
 *  extreme_det() 拐点检测
 *
 *  Note that the filter delay is 1
 ******************************************************************************/

int16_t extreme_det(int16_t datum, int32_t init)
{
	static int16_t last_datum = 0;
	static int16_t max_datum = 0;
	static uint8_t DIFF_MAX_cur_pnt = 0;
	static int16_t maxima_sig_diff[2] = {0};
	int16_t slew_RS = 0, slew_QR = 0;
	int16_t temp_sum = 0;
	if (init)
	{
		last_datum = 0;
		max_datum = 0;
		DIFF_MAX_cur_pnt = 0;
		memset(maxima_sig_diff, 0, sizeof(maxima_sig_diff));
		return (0);
	}

	if ((last_datum >= 0 && datum < 0) || (last_datum < 0 && datum >= 0))
	{
		maxima_sig_diff[DIFF_MAX_cur_pnt++] = max_datum;
		DIFF_MAX_cur_pnt %= 2;
		switch (DIFF_MAX_cur_pnt)
		{
		case 0:
			slew_RS = maxima_sig_diff[1];
			slew_QR = maxima_sig_diff[0];
			break;
		case 1:
			slew_RS = maxima_sig_diff[0];
			slew_QR = maxima_sig_diff[1];
			break;
		}
		temp_sum = slew_QR - slew_RS;
		if (temp_sum == 0)
			temp_sum = 1;
		max_datum = datum;
	}
	else
	{
		max_datum = abs(datum) > abs(max_datum) ? datum : max_datum;
	}
	last_datum = datum;
	return temp_sum;
}

static int compar(const void *a, const void *b)
{
	int16_t *ptr_a = (int16_t *)a;
	int16_t *ptr_b = (int16_t *)b;

	return (*ptr_a - *ptr_b);
}

int16_t median(int16_t *array, uint32_t nitems)
{
	qsort(array, nitems, sizeof(int16_t), compar);
	return (array[nitems / 2]);
}

static void RRUpdate(int RRNew)
{
	heart_Rate = (uint16_t)(60 * SAMPLE_RATE / RRNew +0.5f);
}



uint16_t get_heart_Rate(void)
{
	return heart_Rate;
}
	