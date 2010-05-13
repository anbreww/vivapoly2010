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

void dac_init(void);
void pots_read_offsets(int8_t *steer_offset, int8_t *gas_offset);
void adc_init(void);
void dac_set_offsets(int8_t steer_value, int8_t gas_value);
void dac_set_gas(uint8_t gas_value);
void dac_set_steer(uint8_t steer_value);
