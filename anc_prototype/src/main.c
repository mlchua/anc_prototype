
#include "asf.h"
#include "conf_board.h"
#include "anc/anc_anc.h"

int main(void)
{

	/* Initialize the SAM system */
	sysclk_init();
	board_init();

	/* Initialize our ANC system */
	initialize_anc();

	while (1) {
		/* Check if ADC sample is done. */
		if (adc_sample.done == ADC_DONE_MASK) {
			adc_sample.done = 0;
		}
	}
	
	return 0;
}
