#include <avr/io.h>
#include "robopoly_i2c.h"
#include "config.h"
#include "nunchuk.h"
#include <stdio.h>
#include <util/delay.h>
//#include "robopoly.h"
#include "gameplay.h"
#include "dac.h"


#define M_forward_MUL	43
#define M_forward_DIV	50
#define OFFSET_forward	-73

#define M_ANGULAR_MUL	22
#define M_ANGULAR_DIV	25
#define OFFSET_ANGULAR	-83

#define CONSIGNE_MAX 60
#define CONSIGNE_STOP 30
#define CONSIGNE_MIN 0

void acc_dec(nunchuk * _nunchuk, int * _forward, int * _angular);

nunchuk my_nunchuk;
int forward = 0;
int angular = 0;

Flags flags;

//extern int blah;


int main(void)
{
	nunchuk_init();
	adc_init();
	dac_init();
	init_flags(&flags);


	while(1)
	{

		read_config(&flags);
		nunchuk_refresh(&my_nunchuk);

		acc_dec(&my_nunchuk, &forward, &angular);

		dac_set_gas(forward, &flags);
		dac_set_steer(angular, &flags);


		_delay_ms(20);
	}


	return 0;
}

void acc_dec(nunchuk * _nunchuk, int * _forward, int * _angular)
{
	long int forward_temp, angular_temp;
	
	forward_temp = ((long int)(_nunchuk->acc_y) * M_forward_MUL) / M_forward_DIV + OFFSET_forward;
	angular_temp = ((long int)(_nunchuk->acc_x) * M_ANGULAR_MUL) / M_ANGULAR_DIV + OFFSET_ANGULAR;

	/* When Z button is released, stop all motors */
	if( (_nunchuk->button_Z) )
	{
		*_forward = CONSIGNE_STOP;
		*_angular = CONSIGNE_STOP;
	}
	else
	{
		*_forward = (int) forward_temp;
		*_angular = (int) angular_temp;
	}
	

	
	if(*_forward > CONSIGNE_MAX)
		*_forward = CONSIGNE_MAX;

	if(*_forward < CONSIGNE_MIN)
		*_forward = CONSIGNE_MIN;

	if(*_angular > CONSIGNE_MAX)
		*_angular = CONSIGNE_MAX;

	if(*_angular < CONSIGNE_MIN)
		*_angular = CONSIGNE_MIN;


	/*char buff[30];

	sprintf(buff, "forward %d angular %d \n", (int) *_forward, (int) *_angular);

	uartSendString(buff);*/
}
