
#include <string.h>
#include "anc_anc.h"

void initialize_anc(void)
{
	
	initialize_adc();
	initialize_dacc();
	
	/* Allocate memory for anc_fxlms */
	memset((void *)&anc_fxlms, 0x00, sizeof(anc_fxlms));
	
	anc_fxlms.C_idx		= 0;
	anc_fxlms.x_idx		= 0;
	anc_fxlms.sx_idx	= 0;
	anc_fxlms.xp_idx	= 0;
	
	memset(anc_fxlms.C, 0, FXLMS_BUF_LEN);
	memset(anc_fxlms.x, 0, FXLMS_BUF_LEN);
	memset(anc_fxlms.sx, 0, FXLMS_BUF_LEN);
	memset(anc_fxlms.xp, 0, FXLMS_BUF_LEN);
		
	anc_fxlms.step		= 0.0000000085;
	
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
	
	for (w_idx = 0; w_idx < 16; ++w_idx)
	{
		predict += anc_fxlms.x[x_idx] * anc_fxlms.W[w_idx];
		if (++x_idx == 16) { x_idx = 0; }
	}
	return (uint16_t) predict;
}

void anc_update( uint16_t residual )
{
	uint16_t i;
	uint16_t idx = anc_fxlms.sx_idx;
	for ( i = 0; i < FXLMS_BUF_LEN; ++i )
	{
		anc_fxlms.xp[anc_fxlms.xp_idx] += anc_fxlms.sx[idx] * anc_fxlms.W[i];
		idx++;
		if ( idx > FXLMS_BUF_LEN - 1 )
		{
			idx = 0;
		}
	}
	
	for ( i = 0; i < FXLMS_BUF_LEN; ++i )
	{
		anc_fxlms.C[i] += ((float) residual - FXLMS_BUF_LEN) / FXLMS_BUF_LEN * anc_fxlms.step *  anc_fxlms.xp[i];
	}
	
	if ( ++anc_fxlms.x_idx == 16 ) { anc_fxlms.x_idx  = 0; }
	if ( ++anc_fxlms.sx_idx == 16 )	{ anc_fxlms.sx_idx = 0; }
	if ( ++anc_fxlms.xp_idx == 16 )	{ anc_fxlms.xp_idx = 0; }
}