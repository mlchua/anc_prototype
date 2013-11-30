
#include <string.h>
#include "anc_adc.h"
#include "dacc.h"

void initialize_adc(void)
{
	/* Enable ADC clock */
	pmc_enable_periph_clk(ID_ADC);
	
	/* ADC memory allocation */
	memset((void *)&adc_sample, 0x00, sizeof(adc_sample));
	
	/* Initialize ADC 
	 *		Formula: ADCClock = MCK / ( (PRESCAL+1) * 2 )
	 */

	/* Formula:
	 *     Startup  Time = startup value / ADCClock
	 */
	adc_init(ADC, sysclk_get_cpu_hz(), 6400000, ADC_STARTUP_TIME_4);
	
	/* Set ADC timing. */
	/* Formula:
	 *     Transfer Time = (TRANSFER * 2 + 3) / ADCClock
	 *     Tracking Time = (TRACKTIM + 1) / ADCClock
	 *     Settling Time = Settling Value / ADCClock
	 */
	adc_configure_timing(ADC, TRACKING_TIME, ADC_SETTLING_TIME_3, TRANSFER_PERIOD);
	
	/* Enable channel number tag */
	adc_enable_tag(ADC);
	
	/* Channels used for ADC */
	enum adc_channel_num_t c_list[NUM_CHANNELS] = {
		ADC_CHANNEL_4,
		ADC_CHANNEL_5
	};
	
	/* Set user defined channel sequence. */
	adc_configure_sequence(ADC, c_list, 2);
	
	/* Enable sequencer */
	adc_start_sequencer(ADC);
	
	/* Enable channels */
	int i;
	for (i = 0; i < NUM_CHANNELS; ++i) {
		adc_enable_channel(ADC, (enum adc_channel_num_t)i);
		adc_sample.ch_num[i] = c_list[i];
		adc_set_channel_input_gain(ADC, c_list[i], ADC_GAINVALUE_0);	/* Set gain to 1 */
		adc_disable_channel_input_offset(ADC, c_list[i]);				/* Disable channel offset */
	}

	/* Disable analog change */
	adc_disable_anch(ADC);	
	
	/* Disable power-saving mode */
	adc_configure_power_save(ADC, 0x00, 0x00);
	
	/* PDC enabled */
	read_adc_buffer(ADC, adc_sample.ch_val, BUFFER_SIZE);
	
	/* Enable PDC channel interrupt */
	adc_enable_interrupt(ADC, ADC_IER_RXBUFF);
	
	/* Enable ADC interrupt */
	NVIC_EnableIRQ(ADC_IRQn);
	
	/* Enable ADC as Free-Running mode */
	adc_configure_trigger(ADC, ADC_TRIG_SW, 0x01);
	
	return;
}

/*
 * \brief Read converted data through PDC channel.
 *
 * \param p_adc			The pointer of ADC peripheral.
 * \param p_s_buffer	The destination buffer.
 * \param ul_size		The size of the buffer.
 */
uint32_t read_adc_buffer(Adc * p_adc, uint16_t * p_s_buffer, uint32_t ul_size)
{
	/* Check if the first PDC bank is free. */
	if ((p_adc->ADC_RCR == 0) && (p_adc->ADC_RNCR == 0)) {
		p_adc->ADC_RPR = (uint32_t) p_s_buffer;
		p_adc->ADC_RCR = ul_size;
		p_adc->ADC_PTCR = ADC_PTCR_RXTEN;

		return 1;
	} 
	/* Check if the second PDC bank is free. */
	else {	
		if (p_adc->ADC_RNCR == 0) {
			p_adc->ADC_RNPR = (uint32_t) p_s_buffer;
			p_adc->ADC_RNCR = ul_size;

			return 1;
		} 
		else {
			return 0;
		}
	}
}
