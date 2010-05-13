#include <avr/io.h>

#include "robopoly_i2c.h"

#include "config.h"
#include "nunchuk.h"
#include <util/delay.h>

void nunchuk_init(void)
{
	i2c_init_master();
}



void nunchuk_refresh(nunchuk * _nunchuk)
{
	
	i2c_send_start_block();

	i2c_send_addr_block(0x52,WRITE);

	_delay_us(12);

	i2c_send_byte_block(0x00);

	i2c_send_stop_block();

	_delay_ms(3);

	i2c_send_start_block();

	i2c_send_addr_block(0x52,READ);

	_delay_us(25);

	int i=0;

	int buffer[8];

	for(i=0;i<7;i++)
	{
		buffer[i] = i2c_read_byte_block(ACK);
		_delay_us(10);
	}

	buffer[7] = i2c_read_byte_block(NACK);
	_delay_us(10);

	i2c_send_stop_block();

	//Debug

	_nunchuk->stick_x = buffer[0];
	_nunchuk->stick_y = buffer[1];
	_nunchuk->acc_x = buffer[2];
	_nunchuk->acc_y = buffer[3];
	_nunchuk->acc_z = buffer[4];
	_nunchuk->button_Z = buffer[5] & 1;
	_nunchuk->button_C = (buffer[5] & (1<<1) ) >> 1;

	_delay_ms(5);
}
