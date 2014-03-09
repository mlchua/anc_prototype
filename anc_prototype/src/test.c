
#include "test.h"
#include "anc/anc_anc.h"

void run_test(void) {
	
	uint16_t input[100];
	
	initialize_sine(input);
	
	uint16_t i = 0;
	uint16_t predict = 0;
	int16_t error = 0;
	for (i = 0; i < 100; ++i) {
		predict = anc_predict(input[i]);
		error = (((int)input[i]) - 2048) + (((int)predict) - 2048);
		error += 2048;
		if (error < 0) {error = 0;}
		else if (error > 4096) {error = 4096;}
		anc_update((uint16_t) error);
	}
}

void initialize_sine(uint16_t input[]) {
	
	input[0]  = 1424;
	input[1]  = 3237;
	input[2]  = 407;
	input[3]  = 3984;
	input[4]  = 1;
	input[5]  = 4012;
	input[6]  = 355;
	input[7]  = 3310;
	input[8]  = 1338;
	input[9]  = 2138;
	input[10] = 2585;
	input[11] = 934;
	input[12] = 3633;
	input[13] = 143;
	input[14] = 4092;
	input[15] = 60;
	input[16] = 3791;
	input[17] = 716;
	input[18] = 2842;
	input[19] = 1867;
	input[20] = 1598;
	input[21] = 3085;
	input[22] = 522;
	input[23] = 3918;
	input[24] = 12;
	input[25] = 4055;
	input[26] = 260;
	input[27] = 3447;
	input[28] = 1171;
	input[29] = 2319;
	input[30] = 2409;
	input[31] = 1090;
	input[32] = 3512;
	input[33] = 217;
	input[34] = 4072;
	input[35] = 25;
	input[36] = 3879;
	input[37] = 584;
	input[38] = 3006;
	input[39] = 1688;
	input[40] = 1777;
	input[41] = 2925;
	input[42] = 648;
	input[43] = 3837;
	input[44] = 40;
	input[45] = 4083;
	input[46] = 178;
	input[47] = 3574;
	input[48] = 1011;
	input[49] = 2497;
	input[50] = 2230;
	input[51] = 1253;
	input[52] = 3380;
	input[53] = 305;
	input[54] = 4036;
	input[55] = 5;
	input[56] = 3953;
	input[57] = 463;
	input[58] = 3162;
	input[59] = 1511;
	input[60] = 1957;
	input[61] = 2759;
	input[62] = 786;
	input[63] = 3742;
	input[64] = 84;
	input[65] = 4095;
	input[66] = 112;
	input[67] = 3688;
	input[68] = 860;
	input[69] = 2672;
	input[70] = 2049;
	input[71] = 1423;
	input[72] = 3238;
	input[73] = 407;
	input[74] = 3985;
	input[75] = 0;
	input[76] = 4012;
	input[77] = 355;
	input[78] = 3309;
	input[79] = 1339;
	input[80] = 2138;
	input[81] = 2586;
	input[82] = 933;
	input[83] = 3633;
	input[84] = 143;
	input[85] = 4092;
	input[86] = 61;
	input[87] = 3790;
	input[88] = 717;
	input[89] = 2841;
	input[90] = 1868;
	input[91] = 1598;
	input[92] = 3086;
	input[93] = 521;
	input[94] = 3918;
	input[95] = 12;
	input[96] = 4055;
	input[97] = 260;
	input[98] = 3447;
	input[99] = 1172;
	
	return;
}