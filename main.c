#include "gpio.h"					//we use F_CPU + gpio functions
#include "delay.h"					//we use software delays
//#include "coretick.h"				//we use dwt
//#include "adc1.h"					//we use adc
#include "dac.h"					//we use DAC
#include "tim6.h"					//we use tim6 - not used
#include "tim7.h"					//we use tim7, for PID actions
//#include "uart.h"					//for debugging only
#include "iPID.h"					//we use integer PID

//hardware configuration
#define LEDG_PORT		GPIOC
#define LEDG			(1<<9)		//green led on PC9 on Discovery board

#define LEDB_PORT		GPIOC
#define LEDB			(1<<8)		//blue led on PC8 on Discovery board

#define AVG_TGT			1000		//desired output
#define AVG_PR			10000		//AVG period
#define AVG_PS			100			//AVG prescaler
//AVG output LSB on PA4, MSB on PA5
//end hardware configuration

//global defines

//global variables
volatile uint32_t AVG_tgt;						//voltage output, in adc format
volatile uint32_t AVG_adc;						//AVG output as measured by the ADC
volatile uint32_t AVG_dac;						//voltage output to DAC

void avg_calc(void);
uint32_t avg_read(void);

//reset Arbitary voltage generator
void avg_init(void) {
	AVG_tgt = AVG_TGT;							//set voltage output target

	//set up dac - dual channel, right alighted, 12-bit
    dac_init();								//reset the dac
    //set io pins for gpioa4/dac1 (LSB) and gpioa5/dac2 (MSB)
    IO_AFPP(GPIOA, 1<<4);					//GIO_AFPP(GPIO_PinDef[4].gpio, GPIO_PinDef[4].mask);	//configure pin 4 as afio output
	IO_AFPP(GPIOA, 1<<5);					//GIO_AFPP(GPIO_PinDef[5].gpio, GPIO_PinDef[5].mask);	//configure pin 5 as afio output

	//set up tim7
	tim7_init(AVG_PS);							//set tim7 prescaler to 1
	tim7_setpr(AVG_PR);						//set tim7 period
	tim7_act(avg_calc);						//install avg_calc as tim7 handler
}

//generate voltage output - lowest 20-bit effective,
//split into 2x12-bit DACs (12-bit, right adjusted)
//mid 4-digits are overlapped
//LSB on pa4/dac1 and MSB on pa5/dac2
void avg_out(uint32_t val) {
	//lsb first, on ch1/PA4
	DAC->DHR12R1 = val & 0x0ffful;			//bound the value to [0..4095]
	//msb second, on ch2/PA5
	DAC->DHR12R2 = (val >> 8 ) & 0x0ffful;	//bound the value to [0..4095]
	DAC->SWTRIGR|= (1<<0) | (1<<1);			//output on DAC1/2 - cleared by hardware
}

//sample avg output voltage
//may implement error correction here (for LTC2400 for example)
uint32_t avg_read(void) {
	//call adc routines here
	return 1024;							//return a fix number, for debugging
}

//calculate DAC values
//tgt: output target, in adc format
//adc: output adc result
void avg_calc(void) {
	static uint32_t tmp=0;					//for test only

	IO_FLP(LEDG_PORT, LEDG);				//flip ledg, for debug only

	//calculate AVG_dac from AVG_tgt and AVG_adc
	AVG_adc = avg_read();					//read the output
	//LPF or iPID here: on AVG_tgt and AVG_adc
	AVG_dac = tmp++;						//for test only - should observe faster sweep on LSB/PA4
	avg_out(AVG_dac);						//effect the DACs
}

int main(void) {
	int i;

	mcu_init();									//reset the  mcu

	//initialize the pins to outout
	IO_OUT(LEDG_PORT, LEDG);					//ledg/b as output
	IO_OUT(LEDB_PORT, LEDB);

	avg_init();									//reset the avg - ran periodicially via tim7 isr

	ei();										//enable global interrupts
	while (1) {
	};
}
