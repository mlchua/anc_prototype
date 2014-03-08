
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
	
	input[0]  = 3771;
	input[1]  = 3910;
	input[2]  = 2337;
	input[3]  = 498;
	input[4]  = 84;
	input[5]  = 1476;
	input[6]  = 3394;
	input[7]  = 4074;
	input[8]  = 2892;
	input[9]  = 934;
	input[10] = 0;
	input[11] = 949;
	input[12] = 2909;
	input[13] = 4077;
	input[14] = 3380;
	input[15] = 1458;
	input[16] = 79;
	input[17] = 510;
	input[18] = 2355;
	input[19] = 3918;
	input[20] = 3761;
	input[21] = 2030;
	input[22] = 315;
	input[23] = 193;
	input[24] = 1777;
	input[25] = 3610;
	input[26] = 4007;
	input[27] = 2603;
	input[28] = 689;
	input[29] = 25;
	input[30] = 1221;
	input[31] = 3177;
	input[32] = 4096;
	input[33] = 3132;
	input[34] = 1171;
	input[35] = 17;
	input[36] = 730;
	input[37] = 2655;
	input[38] = 4022;
	input[39] = 3574;
	input[40] = 1723;
	input[41] = 171;
	input[42] = 345;
	input[43] = 2084;
	input[44] = 3791;
	input[45] = 3895;
	input[46] = 2301;
	input[47] = 475;
	input[48] = 95;
	input[49] = 1511;
	input[50] = 3421;
	input[51] = 4069;
	input[52] = 2859;
	input[53] = 904;
	input[54] = 1;
	input[55] = 980;
	input[56] = 2941;
	input[57] = 4081;
	input[58] = 3352;
	input[59] = 1424;
	input[60] = 69;
	input[61] = 534;
	input[62] = 2391;
	input[63] = 3932;
	input[64] = 3741;
	input[65] = 1994;
	input[66] = 296;
	input[67] = 209;
	input[68] = 1813;
	input[69] = 3633;
	input[70] = 3996;
	input[71] = 2568;
	input[72] = 662;
	input[73] = 30;
	input[74] = 1254;
	input[75] = 3207;
	input[76] = 4095;
	input[77] = 3101;
	input[78] = 1138;
	input[79] = 13;
	input[80] = 758;
	input[81] = 2689;
	input[82] = 4031;
	input[83] = 3550;
	input[84] = 1687;
	input[85] = 157;
	input[86] = 365;
	input[87] = 2120;
	input[88] = 3809;
	input[89] = 3879;
	input[90] = 2265;
	input[91] = 452;
	input[92] = 106;
	input[93] = 1546;
	input[94] = 3447;
	input[95] = 4062;
	input[96] = 2825;
	input[97] = 874;
	input[98] = 2;
	input[99] = 1011;
	
	return;
}