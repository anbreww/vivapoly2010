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

	uint8_t gas = 0;

	while(1)
	{
		++gas;
		if(gas > 50)
			gas = 0;
		_delay_ms(200);
		dac_set_gas(gas);
		dac_set_steer(gas);
	}
}
