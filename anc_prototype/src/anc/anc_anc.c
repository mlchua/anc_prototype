
#include <string.h>
#include "anc_anc.h"

void initialize_anc(void)
{	
	/* Allocate memory for anc_fxlms */
	memset((void *)&anc_fxlms, 0x00, sizeof(anc_fxlms));
	
	anc_fxlms.x_idx	= 0;

	memset(anc_fxlms.x, 0, FXLMS_BUF_LEN);
		
	anc_fxlms.step	= FXLMS_STEP;
	
	/* set weights to pseudo-random values */
	anc_fxlms.W[0]  = -0.8049;
	anc_fxlms.W[1]  = -0.4430;
	anc_fxlms.W[2]  = 0.0938;
	anc_fxlms.W[3]  = 0.9150;
	anc_fxlms.W[4]  = 0.9298;
	
	//initialize_dacc();
	//initialize_adc();
}

uint16_t anc_predict( uint16_t noise )
{
	anc_fxlms.x[anc_fxlms.x_idx] = ((float) noise) - 2048;
	
	float predict = 0;
	uint16_t w_idx = 0;
	uint16_t x_idx = anc_fxlms.x_idx;
	
	for (w_idx = 0; w_idx < FXLMS_BUF_LEN; ++w_idx) {
		predict += anc_fxlms.x[x_idx] * -1 * anc_fxlms.W[w_idx];
		if (x_idx == 0) {x_idx = FXLMS_BUF_LEN - 1;}
		else {--x_idx;}
	}
	
	/* explicitly limit prediction based
		on max/min values for DAC */
	if (predict > FXLMS_MID) { 
		predict = FXLMS_MID; 
	}
	else if (predict < -1 * FXLMS_MID) {
		predict = -1 * FXLMS_MID; 
	}  
	
	return (uint16_t) (predict + 2048);
}

void anc_update( uint16_t residual )
{
	const float step = anc_fxlms.step;
	const float error = (float) residual - 2048;
	
	uint16_t x_idx = anc_fxlms.x_idx;
	float buff_step[FXLMS_BUF_LEN];
	uint16_t buff_idx = 0;
	for (buff_idx = 0; buff_idx < FXLMS_BUF_LEN; ++buff_idx) {
		buff_step[buff_idx] = 0;
	}
	
	uint16_t i = 0;
	for (i = 0; i < FXLMS_BUF_LEN; ++i) {
		buff_step[i] = step * (float) error * anc_fxlms.x[x_idx];
		if (x_idx == 0) {x_idx = FXLMS_BUF_LEN - 1;}
		else {--x_idx;}
	}
	
	uint16_t w_idx = 0;
	for (w_idx = 0; w_idx < FXLMS_BUF_LEN; ++w_idx) {
		anc_fxlms.W[w_idx] = (1 - step) * anc_fxlms.W[w_idx] + buff_step[w_idx];
	}
	
	if (++anc_fxlms.x_idx == FXLMS_BUF_LEN) {anc_fxlms.x_idx = 0;}
}