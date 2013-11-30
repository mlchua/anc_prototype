
#ifndef ANC_DACC_H_
#define ANC_DACC_H_

/* DACC Constants */
#define MAX_DIGITAL			4095
#define MAX_DIGITAL_DAC		0x7ff
#define MAX_AMPLITUDE		DACC_MAX_DATA
#define MIN_AMPLITUDE		100
#define DACC_ANALOG_CONTROL ( DACC_ACR_IBCTLCH0(0x02) \
							| DACC_ACR_IBCTLCH1(0x02) \
							| DACC_ACR_IBCTLDACCORE(0x01))

void initialize_dacc(void);



#endif /* ANC_DACC_H_ */