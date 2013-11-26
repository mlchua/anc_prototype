/**
 * \file
 *
 * \brief Analog-to-Digital Converter (ADC/ADC12B) example for SAM.
 *
 * Copyright (c) 2011 - 2013 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include <string.h>
#include "asf.h"
#include "conf_board.h"

/*
 * We use two ADC channels for this example:
 *    ADC_CHANNEL_5  (potentiometer)
 *    ADC_CHANNEL_15 (temperature sensor)
 */

/* Tracking Time*/
#define  TRACKING_TIME            1

/* Transfer Period */
#define  TRANSFER_PERIOD       1 

/** Total number of ADC channels in use */
#define NUM_CHANNELS    (2)

/** ADC convention done mask. */
#define ADC_DONE_MASK   ( (1<<NUM_CHANNELS) - 1 )

/** Size of the receive buffer and transmit buffer. */
#define BUFFER_SIZE     NUM_CHANNELS

/** The maximal digital value */
#define MAX_DIGITAL     (4095)

/** ADC test mode structure */
struct {
	uint8_t uc_trigger_mode;
	uint8_t uc_pdc_en;
	uint8_t uc_sequence_en;
	uint8_t uc_gain_en;
	uint8_t uc_offset_en;
	uint8_t uc_power_save_en;
	uint8_t uc_auto_calib_en;
} g_adc_test_mode;

/** ADC trigger modes */
enum {
	TRIGGER_MODE_SOFTWARE = 0,
	TRIGGER_MODE_ADTRG,
	TRIGGER_MODE_TIMER,
	TRIGGER_MODE_PWM,
	TRIGGER_MODE_FREERUN
} e_trigger_mode;

/** ADC sample data */
struct {
	uint8_t uc_ch_num[NUM_CHANNELS];
	uint16_t us_value[NUM_CHANNELS];
	uint16_t us_done;
} g_adc_sample_data;

/**Channel list for sequence*/
enum adc_channel_num_t ch_list[2] = {
	ADC_CHANNEL_4,
	ADC_CHANNEL_5
};

/** Global timestamp in milliseconds since start of application */
static volatile uint32_t gs_ul_ms_ticks = 0;



/** Analog control value */
#define DACC_ANALOG_CONTROL (DACC_ACR_IBCTLCH0(0x02) \
| DACC_ACR_IBCTLCH1(0x02) \
| DACC_ACR_IBCTLDACCORE(0x01))

/** The maximal sine wave sample data (no sign) */
#define MAX_DIGITAL_DAC   (0x7ff)
/** The maximal (peak-peak) amplitude value */
#define MAX_AMPLITUDE (DACC_MAX_DATA)
/** The minimal (peak-peak) amplitude value */
#define MIN_AMPLITUDE (100)

/*! Convert wave data to DACC value
 *  Put the sinewave to an offset of max_amplitude/2.
 *  \param wave          Waveform data
 *  \param amplitude     Amplitude value
 *  \param max_digital   Maximal digital value of input data (no sign)
 *  \param max_amplitude Maximal amplitude value
 */
#define wave_to_dacc(wave, amplitude, max_digital, max_amplitude) \
	(((int)(wave) * (amplitude) / (max_digital)) + (max_amplitude / 2))









/**
 * \brief Read converted data through PDC channel.
 *
 * \param p_adc The pointer of adc peripheral.
 * \param p_s_buffer The destination buffer.
 * \param ul_size The size of the buffer.
 */
static uint32_t adc_read_buffer(Adc * p_adc, uint16_t * p_s_buffer, uint32_t ul_size)
{
	/* Check if the first PDC bank is free. */
	if ((p_adc->ADC_RCR == 0) && (p_adc->ADC_RNCR == 0)) {
		p_adc->ADC_RPR = (uint32_t) p_s_buffer;
		p_adc->ADC_RCR = ul_size;
		p_adc->ADC_PTCR = ADC_PTCR_RXTEN;

		return 1;
	} else {	/* Check if the second PDC bank is free. */
		if (p_adc->ADC_RNCR == 0) {
			p_adc->ADC_RNPR = (uint32_t) p_s_buffer;
			p_adc->ADC_RNCR = ul_size;

			return 1;
		} else {
			return 0;
		}
	}
}

/**
 * \brief Start ADC sample.
 * Initialize ADC, set clock and timing, and set ADC to given mode.
 */
static void start_adc(void)
{
	/* Enable peripheral clock. */
	uint32_t i;
	pmc_enable_periph_clk(ID_ADC);
	pmc_enable_periph_clk(ID_DACC);

	/* Initialize ADC. */
	/*
	 * Formula: ADCClock = MCK / ( (PRESCAL+1) * 2 )
	 * For example, MCK = 64MHZ, PRESCAL = 4, then:
	 * ADCClock = 64 / ((4+1) * 2) = 6.4MHz;
	 */

	/* Formula:
	 *     Startup  Time = startup value / ADCClock
	 *     Startup time = 64 / 6.4MHz = 10 us
	 */
	adc_init(ADC, sysclk_get_cpu_hz(), 6400000, ADC_STARTUP_TIME_4);
	
	
	/* Reset DACC registers */
	dacc_reset(DACC);

	/* Half word transfer mode */
	dacc_set_transfer_mode(DACC, 0);
	
	/* Power save:
	 * sleep mode  - 0 (disabled)
	 * fast wakeup - 0 (disabled)
	 */
	dacc_set_power_save(DACC, 0, 0);
	/* Timing:
	 * refresh        - 0x08 (1024*8 dacc clocks)
	 * max speed mode -    0 (disabled)
	 * startup time   - 0x10 (1024 dacc clocks)
	 */
	dacc_set_timing(DACC, 0x08, 0, 0x10);

	/* Disable TAG and select output channel DACC_CHANNEL */
	dacc_set_channel_selection(DACC, 0x01);

	/* Enable output channel DACC_CHANNEL */
	dacc_enable_channel(DACC, 0x01);

	/* Set up analog current */
	dacc_set_analog_control(DACC, DACC_ANALOG_CONTROL);

	/* ADC memory allocation */
	memset((void *)&g_adc_sample_data, 0, sizeof(g_adc_sample_data));

	/* Set ADC timing. */
	/* Formula:
	 *     Transfer Time = (TRANSFER * 2 + 3) / ADCClock
	 *     Tracking Time = (TRACKTIM + 1) / ADCClock
	 *     Settling Time = settling value / ADCClock
	 *
	 *     Transfer Time = (1 * 2 + 3) / 6.4MHz = 781 ns
	 *     Tracking Time = (1 + 1) / 6.4MHz = 312 ns
	 *     Settling Time = 3 / 6.4MHz = 469 ns
	 */
	adc_configure_timing(ADC, TRACKING_TIME, ADC_SETTLING_TIME_3, TRANSFER_PERIOD);

	/* Enable channel number tag. */
	adc_enable_tag(ADC);
	
	/* Enable/disable sequencer. */
	if (g_adc_test_mode.uc_sequence_en) {
		/* Set user defined channel sequence. */
		adc_configure_sequence(ADC, ch_list, 2);

		/* Enable sequencer. */
		adc_start_sequencer(ADC);

		/* Enable channels. */
		for (i = 0; i < 2; i++) {
			adc_enable_channel(ADC, (enum adc_channel_num_t)i);
		}
		/* Update channel number. */
		g_adc_sample_data.uc_ch_num[0] = ch_list[0];
		g_adc_sample_data.uc_ch_num[1] = ch_list[1];
	} else {
		/* Disable sequencer. */
		adc_stop_sequencer(ADC);

		/* Enable channels. */
		adc_enable_channel(ADC, ADC_CHANNEL_5);
		adc_enable_channel(ADC, ADC_CHANNEL_4);

		/* Update channel number. */
		g_adc_sample_data.uc_ch_num[0] = ADC_CHANNEL_5;
		g_adc_sample_data.uc_ch_num[1] = ADC_CHANNEL_4;

	}
	
	/* Set gain and offset (only single ended mode used here). */

	adc_disable_anch(ADC);	/* Disable analog change. */

	if (g_adc_test_mode.uc_gain_en) {

		adc_enable_anch(ADC);
		/* gain = 2 */
		adc_set_channel_input_gain(ADC, ADC_CHANNEL_5, ADC_GAINVALUE_2);

	} else {
		/* gain = 1 */
		adc_set_channel_input_gain(ADC, ADC_CHANNEL_5, ADC_GAINVALUE_0);
		adc_set_channel_input_gain(ADC, ADC_CHANNEL_4, ADC_GAINVALUE_0);
	}

	if (g_adc_test_mode.uc_offset_en) {
		adc_enable_anch(ADC);
		adc_enable_channel_input_offset(ADC, ADC_CHANNEL_5);
	} else {
		adc_disable_channel_input_offset(ADC, ADC_CHANNEL_5);
		adc_disable_channel_input_offset(ADC, ADC_CHANNEL_4);
	}
	
	/* Set Auto Calibration Mode. */
	if (g_adc_test_mode.uc_auto_calib_en) {
		adc_set_calibmode(ADC);
		while (1) {
			if ((adc_get_status(ADC) & ADC_ISR_EOCAL) ==
					ADC_ISR_EOCAL)
				break;
		}
	}

	/* Set power save. */
	if (g_adc_test_mode.uc_power_save_en) {
		adc_configure_power_save(ADC, 1, 0);
	} else {
		adc_configure_power_save(ADC, 0, 0);;
	}


	/* Transfer with/without PDC. */
	if (g_adc_test_mode.uc_pdc_en) {
		adc_read_buffer(ADC, g_adc_sample_data.us_value, BUFFER_SIZE);
		/* Enable PDC channel interrupt. */
		adc_enable_interrupt(ADC, ADC_IER_RXBUFF);
	} else {
		/* Enable Data ready interrupt. */
		adc_enable_interrupt(ADC, ADC_IER_DRDY);
	}
	/* Enable ADC interrupt. */
	NVIC_EnableIRQ(ADC_IRQn);

	/* Configure trigger mode and start convention. */
	switch (g_adc_test_mode.uc_trigger_mode) {
	case TRIGGER_MODE_SOFTWARE:
		adc_configure_trigger(ADC, ADC_TRIG_SW, 0);	/* Disable hardware trigger. */
		break;

	case TRIGGER_MODE_ADTRG:
		gpio_configure_pin( PIO_PA8_IDX, (PIO_PERIPH_B | PIO_DEFAULT) );

		adc_configure_trigger(ADC, ADC_TRIG_EXT, 0);
		break;

	case TRIGGER_MODE_FREERUN:
		adc_configure_trigger(ADC, ADC_TRIG_SW, 1);
		break;
		
	default:
		break;
	}
}


float cx[16] = { 0 };
uint16_t cxIndex = 0;
float cw[16] = { 0 };
uint16_t cwIndex = 0;
float shx[16] = { 0 };
uint16_t shxIndex = 0;
float shw[16] = {		0.0025,
						0.0625,
						0.1250,
						0.2500,
						0.1250,
						0.0625,
						0.0025,
					   -0.00000000030657,
						0.0000000026680,
					   -0.00000000018176,
						0.0000000015666,
					   -0.0000000043996,
					   -0.00000000084479,
						0.0000000017471,
					   -0.0000000030584,
						0.0000000066447,
					};
float xhx[16] = { 0 };
uint16_t xhxIndex = 0;
const float step = 0.1;

static uint16_t anc_predict( uint16_t dac_val )
{
	cx[cxIndex] = ( (float) dac_val - 2048 ) / 2048;
	float predict = 0;
	uint16_t i;
	for ( i = 0; i < 16; ++i )
	{
		predict += cx[i] * cw[i];
	}
	return (uint16_t) ( predict * 2048 ) + 2048;
}

static void anc_update( uint16_t input_dac, uint16_t error_dac )
{
	shx[shxIndex] = ( (float) input_dac - 2048 ) / 2048;
	
	uint16_t i;
	uint16_t idx = shxIndex;
	float filter = 0;
	for ( i = 0; i < 16; ++i )
	{
		filter = shx[idx] * shw[i];
		idx++;
		if ( idx > 15 )
		{
			idx = 0;
		}
	}
	xhx[xhxIndex] = filter;
	
	for ( i = 0; i < 16; ++i )
	{
		cw[i] += ((float) error_dac - 2048) / 2048 * step *  xhx[i];
	}
	if ( ++cxIndex == 16 ) cxIndex = 0;
	if ( ++shxIndex == 16 ) shxIndex = 0;
	if ( ++xhxIndex == 16 ) xhxIndex = 0;
}



/**
 * \brief Interrupt handler for the ADC.
 */
void ADC_Handler(void)
{
	uint32_t i;
	uint32_t ul_temp;
	uint8_t uc_ch_num;

	uint16_t temp[NUM_CHANNELS];

	/* With PDC transfer */
	if (g_adc_test_mode.uc_pdc_en) {
		if ((adc_get_status(ADC) & ADC_ISR_RXBUFF) ==
				ADC_ISR_RXBUFF) {
			g_adc_sample_data.us_done = ADC_DONE_MASK;
			adc_read_buffer(ADC, temp, BUFFER_SIZE);
			
			for ( i = 0; i < NUM_CHANNELS; ++i )
			{
				switch ( ( temp[i] & ADC_LCDR_CHNB_Msk ) >> 0x0C ){
				
				case ADC_CHANNEL_4:
					g_adc_sample_data.us_value[0] = temp[i] & ADC_LCDR_LDATA_Msk;
					break;
					
				case  ADC_CHANNEL_5:
					g_adc_sample_data.us_value[1] = temp[i] & ADC_LCDR_LDATA_Msk;
					break;
				}
			}
			
		}
	} else {	/* Without PDC transfer */
		if ((adc_get_status(ADC) & ADC_ISR_DRDY) ==
				ADC_ISR_DRDY) {
			ul_temp = adc_get_latest_value(ADC);
			for (i = 0; i < NUM_CHANNELS; i++) {
				uc_ch_num = (ul_temp & ADC_LCDR_CHNB_Msk) >>
						ADC_LCDR_CHNB_Pos;
				if (g_adc_sample_data.uc_ch_num[i] == uc_ch_num) {
					g_adc_sample_data.us_value[i] =
							ul_temp &
							ADC_LCDR_LDATA_Msk;
					g_adc_sample_data.us_done |= 1 << i;
				}
			}
		}
	}
	
	
	uint16_t status;
	uint16_t dac_val;

	status = dacc_get_interrupt_status(DACC);

	/* If ready for new data */
	if ((status & DACC_ISR_TXRDY) == DACC_ISR_TXRDY) {

		int predict = anc_predict( g_adc_sample_data.us_value[0] );
		dacc_write_conversion_data(DACC, predict);
		anc_update(g_adc_sample_data.us_value[0], g_adc_sample_data.us_value[1] );
	}
}














/**
 *  Wait for the given number of milliseconds (using the dwTimeStamp generated
 *  by the SAM microcontrollers' system tick).
 *  \param ul_dly_ticks  Delay to wait for, in milliseconds.
 */
static void mdelay(uint32_t ul_dly_ticks)
{
	uint32_t ul_cur_ticks;

	ul_cur_ticks = gs_ul_ms_ticks;
	while ((gs_ul_ms_ticks - ul_cur_ticks) < ul_dly_ticks);
}

/**
 *  \brief adc12 Application entry point.
 *
 *  \return Unused (ANSI-C compatibility).
 */
int main(void)
{

	/* Initialize the SAM system. */
	sysclk_init();
	board_init();

	/* Set default ADC test mode. */
	memset((void *)&g_adc_test_mode, 0, sizeof(g_adc_test_mode));
	g_adc_test_mode.uc_trigger_mode = TRIGGER_MODE_FREERUN;
	g_adc_test_mode.uc_pdc_en = 1;
	g_adc_test_mode.uc_sequence_en = 1;
	g_adc_test_mode.uc_gain_en = 0;
	g_adc_test_mode.uc_offset_en = 0;
	g_adc_test_mode.uc_power_save_en = 0;

	start_adc();

	while (1) {
		/* ADC software trigger per 1s */
		if (g_adc_test_mode.uc_trigger_mode == TRIGGER_MODE_SOFTWARE) {
			mdelay(125);
			adc_start(ADC);
		}

		/* Check if ADC sample is done. */
		if (g_adc_sample_data.us_done == ADC_DONE_MASK) {
			g_adc_sample_data.us_done = 0;
		}
	}
}
