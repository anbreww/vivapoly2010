#include "config.h"
//#include "robopoly_adc.h"
#include <avr/io.h>
#include <util/delay.h>


/*
 * DAC test for Vivapoly
 *
 * Purpose : functions to generate analog values using a filtered PWM output.
 * Test program tests various combinations. Low speed, high speed, pan wheels
 * left to right, etc.
 *
 */

#define STEER_PIN	PD4 // B
#define GAS_PIN		PD5 // A
#define DAC_PORT	PORTD
#define DAC_DDR		DDRD

#define ADC_PORT	PORTA
#define TRIM_STEER	PA7	/* pot to set steering offset */
#define TRIM_GAS	PA6	/* pot to set gas offset */

//static uint8_t _steer_offset;
//static uint8_t _gas_offset;

void dac_init(void)
{
	// init PWM
	DAC_PORT = 0;
	DAC_DDR |= _BV(STEER_PIN) | _BV(GAS_PIN);

	// set clock source, fast PWM mode
	TCCR1A = (0b10 << COM1A0)  	// non-inverting
		| (0b10 << COM1B0)
		| (0b01 << WGM10) 	// 8-bit mode
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
void pots_read_offsets(int8_t *steer_offset, int8_t *gas_offset)
{
	uint8_t steer_adc, gas_adc;

	/* get results from ADC */
	ADCSRA = 0x87; 

	ADMUX =0x20+7;  // STEERING BIT
	ADCSRA |= (1<<ADSC); 
	while((ADCSRA & (1<<ADSC))>>ADSC);

	steer_adc = ADCH; 

	ADMUX =0x20+6;  // STEERING BIT
	ADCSRA |= (1<<ADSC); 
	while((ADCSRA & (1<<ADSC))>>ADSC);

	gas_adc = ADCH;

	ADCSRA = 0;

	/* calculate offset */

	*steer_offset = 128 - steer_adc;
	*gas_offset = 128 - gas_adc;

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
	OCR1B = 0x07FF;
	return;
}

/*
 * set car's speed. 0 is stop, positive = forward
 * adjust with potentiometers until zero == halt
 */
#define GAS_LOW 100
#define GAS_HIGH 150
void dac_set_gas(uint8_t gas_value)
{
	uint16_t pwm_val;

	gas_value += 100;

	if (gas_value > GAS_HIGH)
		pwm_val = GAS_HIGH;
	else if (gas_value < GAS_LOW)
		pwm_val = GAS_LOW;
	else
		pwm_val = gas_value;
	
	// set GAS PWM
	OCR1A = pwm_val;
	return;
}

/*
 * set car's direction. 0 should be center, positive = right
 * adjust with potentiometer until zero centers wheels
 */
#define STEER_LOW 100
#define STEER_HIGH 150
void dac_set_steer(uint8_t steer_value)
{
	uint16_t pwm_val = 0;

	steer_value += 100;

	if (steer_value > STEER_HIGH)
		pwm_val = STEER_HIGH;
	else if (steer_value < STEER_LOW)
		pwm_val = STEER_LOW;
	else
		pwm_val = steer_value;
	
	// set Steering PWM
	OCR1B = pwm_val;
	return;
}


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
