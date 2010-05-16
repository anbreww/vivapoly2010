#include "config.h"
//#include "robopoly_adc.h"
#include <avr/io.h>
#include <util/delay.h>

#include "dac.h"




int main (void)
{
	DDRA = 0x00; // ADC port as inputs for bias pots
	dac_init();
	adc_init();

	uint8_t gas = 25;

	while(1)
	{
		//++gas;
		if(gas > 50)
			gas = 0;
		_delay_ms(1);
		dac_set_gas(gas);
		dac_set_steer(gas);
	//OCR1A = 0x01FF;
	//OCR1B = 0x007F;
	}
}
