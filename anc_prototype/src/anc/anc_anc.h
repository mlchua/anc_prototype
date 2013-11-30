
#ifndef ANC_ANC_H_
#define ANC_ANC_H_

#include "anc/anc_adc.h"
#include "anc/anc_dacc.h"

#define FXLMS_BUF_LEN	16
#define FXLMS_STEP		0.1
#define FXLMS_ZERO		2048

struct {
	uint16_t cx_idx;
	uint16_t cw_idx;
	uint16_t shx_idx;
	uint16_t xhx_idx;
	
	float step;
	
	float cx[FXLMS_BUF_LEN];
	float cw[FXLMS_BUF_LEN];
	float shx[FXLMS_BUF_LEN];
	float xhx[FXLMS_BUF_LEN];
	float shw[FXLMS_BUF_LEN];
	
} anc_fxlms;

void initialize_anc(void);
uint16_t anc_predict( uint16_t noise );
void anc_update( uint16_t residual );

#endif 