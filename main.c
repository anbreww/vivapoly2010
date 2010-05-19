#include <avr/io.h>
#include "robopoly_i2c.h"
#include "config.h"
#include "nunchuk.h"
#include <stdio.h>
#include <util/delay.h>
//#include "robopoly.h"
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

struct FLAGS
{
	int boost_available:1;
	int boost_enabled:1;
} flags;

void init_flags(void);

int main(void)
{
	nunchuk_init();
	adc_init();
	dac_init();
	init_flags();


	while(1)
	{
		nunchuk_refresh(&my_nunchuk);

		acc_dec(&my_nunchuk, &forward, &angular);

		dac_set_gas(forward);
		dac_set_steer(angular);


		_delay_ms(20);
	}


	return 0;
}

void init_flags(void)
{
	flags.boost_available = 1;
	flags.boost_enabled = 0;
}
/*
 * Boost timer, step one : boost is active for a short while
 * Short timer interrupt disables boost then launches a longer timer.
 * At the end of long period, boost is re-enabled
 */
#define BOOST_DURATION 3
#define INTERVAL_BETWEEN_BOOSTS	15
void call_booster(void)
{
	/* are we allowed to use the booster yet? */
	if (flags.boost_available && !flags.boost_enabled)
	{
		/* not available until timeout */
		flags.boost_available = 0;
		/* give a few seconds of boost */
		flags.boost_enabled = 1;
		/* start timer for deactivation */
		// TODO
	}
	/* all other cases : just wait for timer to time out */
	return;
}

/* function that gets called by booster timer overflow */
// TODO : rename this function
void overflow_callback(void)
{
	/* if booster is currently in use */
	if (flags.boost_enabled)
	{
		flags.boost_enabled = 0;
		/* start long timer to re-enable */
		// TODO
	}
	else
	{
		/* we've waited long enough : let's have some boost again! */
		flags.boost_available = 1;
	}
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
