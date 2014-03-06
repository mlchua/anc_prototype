
#ifndef ANC_ANC_H_
#define ANC_ANC_H_

#include "anc/anc_adc.h"
#include "anc/anc_dacc.h"

#define FXLMS_BUF_LEN	16
#define FXLMS_STEP		0.1
#define FXLMS_ZERO		2048

struct {
	uint16_t x_idx;
	uint16_t C_idx;
	//uint16_t sx_idx;
	uint16_t xp_idx;
	
	float step;
	
	float x[FXLMS_BUF_LEN];
	float C[FXLMS_BUF_LEN];
	float sx[FXLMS_BUF_LEN];
	float xp[FXLMS_BUF_LEN];
	float W[FXLMS_BUF_LEN];
	
} anc_fxlms;

void initialize_anc(void);
uint16_t anc_predict( uint16_t noise );
void anc_update( uint16_t residual );

#endif 