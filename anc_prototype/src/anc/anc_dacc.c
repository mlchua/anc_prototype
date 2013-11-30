
#include "pmc.h"
#include "dacc.h"
#include "anc_dacc.h"

void initialize_dacc(void)
{
	/* Enable DACC clock */
	pmc_enable_periph_clk(ID_DACC);
	
	/* Reset DACC registers */
	dacc_reset(DACC);
	
	/* Half-Word transfer mode */
	dacc_set_transfer_mode(DACC, 0x00);
	
	/* Power save:
	 * sleep mode  - 0 (disabled)
	 * fast wakeup - 0 (disabled)
	 */
	dacc_set_power_save(DACC, 0x00, 0x00);
	/* Timing:
	 * refresh        - 0x08 (1024*8 dacc clocks)
	 * max speed mode -    0 (disabled)
	 * startup time   - 0x10 (1024 dacc clocks)
	 */
	dacc_set_timing(DACC, 0x08, 0x00, 0x10);
	
	/* Disable TAG and select output channel DACC_CHANNEL */
	dacc_set_channel_selection(DACC, 0x01);

	/* Enable output channel DACC_CHANNEL */
	dacc_enable_channel(DACC, 0x01);

	/* Set up analog current */
	dacc_set_analog_control(DACC, DACC_ANALOG_CONTROL);
	
	
}