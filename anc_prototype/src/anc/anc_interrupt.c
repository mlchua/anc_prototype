
#include "dacc.h"
#include "anc_interrupt.h"

void ADC_Handler(void)
{
	uint16_t temp[NUM_CHANNELS];

	if ((adc_get_status(ADC) & ADC_ISR_RXBUFF) == ADC_ISR_RXBUFF) {
		adc_sample.done = ADC_DONE_MASK;
		read_adc_buffer(ADC, temp, BUFFER_SIZE);
		
		uint32_t i;
		for ( i = 0; i < NUM_CHANNELS; ++i ) {
			switch ( ( temp[i] & ADC_LCDR_CHNB_Msk ) >> 0x0C ) {
				case ADC_CHANNEL_4:
				adc_sample.ch_val[0] = temp[i] & ADC_LCDR_LDATA_Msk;
				break;
				
				case ADC_CHANNEL_5:
				adc_sample.ch_val[1] = temp[i] & ADC_LCDR_LDATA_Msk;
				break;
			}
		}
		
	}
	
	uint16_t status = dacc_get_interrupt_status(DACC);

	/* If ready for new data */
	if ((status & DACC_ISR_TXRDY) == DACC_ISR_TXRDY) {
		int16_t predict = anc_predict( adc_sample.ch_val[0] );
		dacc_write_conversion_data(DACC, predict);
		if (adc_sample.idx < 2000) {
			adc_sample.pre[adc_sample.idx] =  predict;
			adc_sample.inp[adc_sample.idx] =  adc_sample.ch_val[0];
			adc_sample.err[adc_sample.idx] =  adc_sample.ch_val[1];
			++adc_sample.idx;
		}
		anc_update(adc_sample.ch_val[1] );
	}
}