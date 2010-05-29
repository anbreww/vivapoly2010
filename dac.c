#include "config.h"
#include <avr/io.h>
#include <util/delay.h>

#include "gameplay.h"
#include "dac.h"

//static uint8_t _steer_offset;
//static uint8_t _gas_offset;
//int blah;

void dac_init(void)
{
	// init PWM
	DAC_PORT = 0;
	DAC_DDR |= _BV(STEER_PIN) | _BV(GAS_PIN);

	// set clock source, fast PWM mode
	TCCR1A = (0b10 << COM1A0)  	// non-inverting
		| (0b10 << COM1B0)
		| (0b11 << WGM10) 	// 8-bit mode
		;
	TCCR1B = (0b01 << WGM12)	// Fast PWM
		| (1 << CS10)		// Prescaler 1
		;

	
	// init ADC port
	DDRA = 0x00;
	PORTA = 0x00;

	return;
}



/*
 * read analog values from potentiometers (to set offsets)
 */
void pots_read_offsets(uint8_t *steer_offset, uint8_t *gas_offset)
{
	uint8_t steer_adc, gas_adc;

	/* get results from ADC */
	ADCSRA = 0x87; 

	ADMUX =0x20+6;  // STEERING BIT
	ADCSRA |= (1<<ADSC); 
	while((ADCSRA & (1<<ADSC))>>ADSC);

	steer_adc = ADCH; 

	ADMUX =0x20+7;  // GAS BIT
	ADCSRA |= (1<<ADSC); 
	while((ADCSRA & (1<<ADSC))>>ADSC);

	gas_adc = ADCH;

	ADCSRA = 0;

	/* calculate offset */

	*steer_offset = steer_adc;
	*gas_offset = gas_adc;

	return;
}

void adc_init(void)
{
	DDRA  &= ~(_BV(TRIM_STEER) | _BV(TRIM_GAS));
	PORTA &= ~(_BV(TRIM_STEER) | _BV(TRIM_GAS));
}


/*
 * store offset value for direction and speed
 */
void dac_set_offsets(int8_t steer_value, int8_t gas_value)
{
	// set PWM A value
	static uint16_t steer = 0;
	OCR1A = steer;
	++steer;
	// set PWM B value
	OCR1B = 0x02FF;
	return;
}

/*
 * set car's speed. 0 is stop, positive = forward
 * adjust with potentiometers until zero == halt
 * gas_value : 1 step = 20mV
 * gas_offset : full range  = 1.2V
 */
#define GAS_LOW 	14 	/* lower bound for input */
#define GAS_HIGH 	46 	/* higher bound for input */
#define GAS_CENTER 	60	/* PWM value for 2.5v */
#define GAS_TOP	 	60	/* top bound, for reversing */
void dac_set_gas(uint8_t gas_value, Flags * _flags)
{
	uint16_t pwm_val;
	uint8_t steer_offset, gas_offset;

	if(_flags->reverse_speed)
	{
		DEBUG_PORT |= (1<<DEBUG_LGREEN);
		gas_value = GAS_TOP - gas_value;
	}
	else
		DEBUG_PORT &= ~(1<<DEBUG_LGREEN);


	/* unorthodox way to get the offset values from potentiometers */
	pots_read_offsets(&steer_offset, &gas_offset);

	/* check input */
	if (gas_value > GAS_HIGH)
		pwm_val = GAS_HIGH;
	else if (gas_value < GAS_LOW)
		pwm_val = GAS_LOW;
	else
		pwm_val = gas_value;

	/* invert controls (speed control is inverted) */
	pwm_val = GAS_TOP - pwm_val;

	/* add hard-coded offset for 2.5v */
	pwm_val += GAS_CENTER;
	
	/* set PWM with previously acquired ADC offset */
	OCR1B = (pwm_val << 2) - 64 + gas_offset/2; 

	return;
}

/*
 * set car's direction. 0 should be center, positive = right
 * adjust with potentiometer until zero centers wheels
 */
#define STEER_LOW 	0	/* lower bound for input */
#define STEER_HIGH 	60	/* higher bound for input */
#define STEER_CENTER 	60	/* pwm value for 2.5v */
#define STEER_TOP	60	/* top bound, for reversing */
void dac_set_steer(uint8_t steer_value, Flags * _flags)
{
	uint16_t pwm_val = 0;
	uint8_t steer_offset, gas_offset;


	if (_flags->reverse_steering)
	{
		DEBUG_PORT |= (1<<DEBUG_RGREEN);
		steer_value = STEER_TOP - steer_value;
	}
	else
		DEBUG_PORT &= ~(1<<DEBUG_RGREEN);


	pots_read_offsets(&steer_offset, &gas_offset);


	if (steer_value > STEER_HIGH)
		pwm_val = STEER_HIGH;
	else if (steer_value < STEER_LOW)
		pwm_val = STEER_LOW;
	else
		pwm_val = steer_value;

	pwm_val = STEER_HIGH - pwm_val;

	pwm_val += STEER_CENTER;
	
	// set Steering PWM
	OCR1A = (pwm_val << 2) - 64 + steer_offset/2;
	return;
}
