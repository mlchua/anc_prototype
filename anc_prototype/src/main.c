
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

	/* Loop indefinitely */
	while (1) {};
	
	return 0;
}
