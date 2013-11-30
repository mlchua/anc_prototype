
#include <string.h>
#include "anc_anc.h"

void initialize_anc(void)
{
	
	initialize_adc();
	initialize_dacc();
	
	/* Allocate memory for anc_fxlms */
	memset((void *)&anc_fxlms, 0x00, sizeof(anc_fxlms));
	
	anc_fxlms.cw_idx	= 0;
	anc_fxlms.cx_idx	= 0;
	anc_fxlms.shx_idx	= 0;
	anc_fxlms.xhx_idx	= 0;
	
	memset(anc_fxlms.cw, 0, FXLMS_BUF_LEN);
	memset(anc_fxlms.cx, 0, FXLMS_BUF_LEN);
	memset(anc_fxlms.shx, 0, FXLMS_BUF_LEN);
	memset(anc_fxlms.xhx, 0, FXLMS_BUF_LEN);
		
	anc_fxlms.step		= 0.1;
	
	anc_fxlms.shw[0]  = 0.0025;
	anc_fxlms.shw[1]  = 0.0625;
	anc_fxlms.shw[2]  = 0.1250;
	anc_fxlms.shw[3]  = 0.2500;
	anc_fxlms.shw[4]  = 0.1250;
	anc_fxlms.shw[5]  = 0.0625;
	anc_fxlms.shw[6]  = 0.0025;
	anc_fxlms.shw[7]  = -0.00000000030657;
	anc_fxlms.shw[8]  = 0.0000000026680;
	anc_fxlms.shw[9]  = -0.00000000018176;
	anc_fxlms.shw[10] = 0.0000000015666;
	anc_fxlms.shw[11] = -0.0000000043996;
	anc_fxlms.shw[12] = -0.00000000084479;
	anc_fxlms.shw[13] = 0.0000000017471;
	anc_fxlms.shw[14] = -0.0000000030584;
	anc_fxlms.shw[15] = 0.0000000066447;
}

uint16_t anc_predict( uint16_t noise )
{
	anc_fxlms.cx[anc_fxlms.cw_idx] = ( (float) noise - FXLMS_ZERO ) / FXLMS_ZERO;
	float predict = 0;
	uint16_t i;
	for ( i = 0; i < 16; ++i )
	{
		predict += anc_fxlms.cx[i] * anc_fxlms.cw[i];
	}
	return (uint16_t) ( predict * FXLMS_ZERO ) + FXLMS_ZERO;
}

void anc_update( uint16_t residual )
{
	anc_fxlms.shx[anc_fxlms.shx_idx] = anc_fxlms.cx[anc_fxlms.cw_idx];
	
	uint16_t i;
	uint16_t idx = anc_fxlms.shx_idx;
	for ( i = 0; i < FXLMS_BUF_LEN; ++i )
	{
		anc_fxlms.xhx[anc_fxlms.xhx_idx] += anc_fxlms.shx[idx] * anc_fxlms.shw[i];
		idx++;
		if ( idx > FXLMS_BUF_LEN - 1 )
		{
			idx = 0;
		}
	}
	
	for ( i = 0; i < FXLMS_BUF_LEN; ++i )
	{
		anc_fxlms.cw[i] += ((float) residual - FXLMS_BUF_LEN) / FXLMS_BUF_LEN * anc_fxlms.step *  anc_fxlms.xhx[i];
	}
	if ( ++anc_fxlms.cx_idx == 16 )		anc_fxlms.cx_idx  = 0;
	if ( ++anc_fxlms.shx_idx == 16 )	anc_fxlms.shx_idx = 0;
	if ( ++anc_fxlms.xhx_idx == 16 )	anc_fxlms.xhx_idx = 0;
}