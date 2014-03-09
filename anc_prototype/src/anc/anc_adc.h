
#ifndef ANC_ADC_H_
#define ANC_ADC_H_

#include "adc.h"
#include "pmc.h"
#include "sysclk.h"

/* ADC Constants */
#define TRACKING_TIME		1
#define TRANSFER_PERIOD		1
#define	NUM_CHANNELS		2
#define BUFFER_SIZE			NUM_CHANNELS
#define ADC_DONE_MASK		((1<<NUM_CHANNELS) - 1)

/* ADC input sample */
struct {
	uint8_t		ch_num[NUM_CHANNELS];
	uint16_t	ch_val[NUM_CHANNELS];
	uint16_t	done;
	uint16_t    idx;
	uint16_t	pre[2000];
	uint16_t	inp[2000];
	uint16_t	err[2000];
}adc_sample;

void initialize_adc(void);
uint32_t read_adc_buffer(Adc * p_adc, uint16_t * p_s_buffer, uint32_t ul_size);

#endif 