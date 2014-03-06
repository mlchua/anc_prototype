
#include <string.h>
#include "anc_anc.h"

void initialize_anc(void)
{
	initialize_adc();
	initialize_dacc();
	
	/* Allocate memory for anc_fxlms */
	memset((void *)&anc_fxlms, 0x00, sizeof(anc_fxlms));
	
	anc_fxlms.x_idx		= 0;

	memset(anc_fxlms.x, 0, FXLMS_BUF_LEN);
		
	anc_fxlms.step		= FXLMS_STEP;
	
	anc_fxlms.W[0]  = 0.0025;
	anc_fxlms.W[1]  = 0.0625;
	anc_fxlms.W[2]  = 0.1250;
	anc_fxlms.W[3]  = 0.2500;
	anc_fxlms.W[4]  = 0.1250;
	anc_fxlms.W[5]  = 0.0625;
	anc_fxlms.W[6]  = 0.0025;
	anc_fxlms.W[7]  = -0.00000000030657;
	anc_fxlms.W[8]  = 0.0000000026680;
	anc_fxlms.W[9]  = -0.00000000018176;
	anc_fxlms.W[10] = 0.0000000015666;
	anc_fxlms.W[11] = -0.0000000043996;
	anc_fxlms.W[12] = -0.00000000084479;
	anc_fxlms.W[13] = 0.0000000017471;
	anc_fxlms.W[14] = -0.0000000030584;
	anc_fxlms.W[15] = 0.0000000066447;
}

uint16_t anc_predict( uint16_t noise )
{
	anc_fxlms.x[anc_fxlms.x_idx] = (float) noise;
	
	float predict = 0;
	uint16_t w_idx = 0;
	uint16_t x_idx = anc_fxlms.x_idx;
	
	for (w_idx = 0; w_idx < FXLMS_BUF_LEN; ++w_idx) {
		predict += anc_fxlms.x[x_idx] * anc_fxlms.W[w_idx];
		if (++x_idx == FXLMS_BUF_LEN) { x_idx = 0; }
	}
	return (uint16_t) predict;
}

void anc_update( uint16_t residual )
{
	float step = anc_fxlms.step;
	
	uint16_t x_idx = 0;
	float buff_step = 0;
	for (x_idx = 0; x_idx < FXLMS_BUF_LEN; ++x_idx) {
		buff_step += step * (float) residual * anc_fxlms.x[x_idx];
	}
	
	uint16_t w_idx = 0;
	for (w_idx = 0; w_idx < FXLMS_BUF_LEN; ++w_idx) {
		anc_fxlms.W[w_idx] = (1 - step) * anc_fxlms.W[w_idx] + buff_step;
	}
}