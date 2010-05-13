#include <avr/io.h>
#include <util/twi.h>
#include "config.h"
#include "robopoly_i2c.h"
//#include "robopoly.h"
//#include "robopoly_uart.h"


void i2c_init_master()
{
	#ifndef F_SCL
	#warning "F_SCL not defined, will be set to 100Khz"
	#endif

	#ifndef F_CPU
	#error "F_CPU must be defined!"
	#endif

	#if F_SCL>400000
	#error "Too high I2C frequency"
	#endif
	
	#if F_SCL<1743
	#error "Too low frequency, not implemented yet!"
	#endif

	#define	 TWBR_VALUE  F_CPU/(2*F_SCL)-8

	TWCR &= ~(1<<TWEN);
	TWBR = TWBR_VALUE;
	TWSR = 0;
	TWCR = (1<<TWEN);
}


/* Init I2C module in slave mode
addr: 7-bit adress of uc (unshifted) */
void i2c_init_slave(unsigned char addr)
{
	TWAR = 2*addr;

}

/* Master function: send a START sequence and hold SCL and SDA low*/
void i2c_send_start_block()
{
	TWCR = (1<<TWINT) +(1<<TWEN) +(1<<TWSTA);
	i2c_wait();
	if((TWSR&0xF8) !=TW_START)
	{
		i2c_error(TW_START,(TWSR&0xF8));
	}

}

void i2c_send_restart_block()
{
	TWCR = (1<<	TWINT) +(1<<TWEN) +(1<<TWSTA);
	i2c_wait();
	if((TWSR&0xF8) !=TW_START)
	{
		i2c_error(TW_REP_START,(TWSR&0xF8));
	}

}
	

/*Master function: send a STOP sequence and set SCL and SDA in opendrain mode*/
void i2c_send_stop_block()
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

/*Master function
arg: byte to be send */
void i2c_send_byte_block(unsigned char arg)
{
	TWDR = arg;
	TWCR = (1<<TWINT)|(1<<TWEN);
	i2c_wait();
	if((TWSR&0xF8) != TW_MT_DATA_ACK)
	{
		i2c_error(TW_MT_DATA_ACK,(TWSR&0xF8));
	}
	

}


/* Master function: Send slave adress
supposed that a START sequence has already happened 
addr: 7-bit slave adress(unshifted)
rw: Read/Write bit */
void i2c_send_addr_block(unsigned char addr,unsigned char rw)
{

		TWDR = (addr<<1) + (rw&1);
		TWCR = (1<<TWINT) |(1<<TWEN);
		i2c_wait();
		if(((TWSR&0xF8) != TW_MT_SLA_ACK) && rw == WRITE)
		{
			i2c_error(TW_MT_SLA_ACK,(TWSR&0xF8));
		}

		else if(((TWSR&0xF8) != TW_MR_SLA_ACK) && (rw == READ))
		{
			i2c_error(TW_MR_SLA_ACK,(TWSR&0xF8));
		}

}


/* Master receiver function
ack: send an ACK or NACK sequence after receiving the byte
return the value of incoming buffer */
unsigned char i2c_read_byte_block(unsigned char ack)
{


	TWCR = (1<<TWINT) |(1<<TWEN);
	if(ack == ACK)
		TWCR|=(1<<TWEA);

	i2c_wait();

	unsigned char temp = TWDR;

	if(((TWSR&0xF8) !=TW_MR_DATA_ACK) && ack == ACK) //ack
	{
		i2c_error(TW_MR_DATA_ACK,(TWSR&0xF8));

	}

	else if(((TWSR&0xF8) !=TW_MR_DATA_NACK) && ack == NACK) // nack
	{
		i2c_error(TW_MR_DATA_NACK,(TWSR&0xF8));
	}


	return temp;
}

/*Slave function, wait for a valid call and send an ACK sequence
return read or write (LSB adress byte) or 255 if adress is false (not this chip called) */
unsigned char i2c_read_addr_slave_block()
{
	TWCR = (1<<TWEA) | (1<<TWEN) |(1<<TWINT); 
	i2c_wait();
	if((TWSR&0xF8) != TW_SR_SLA_ACK && (TWSR&0xF8) != TW_ST_SLA_ACK)
	{
		i2c_error(TW_SR_SLA_ACK,(TWSR&0xF8));
		return 0xFF;
	}

	else
	{
		if((TWSR&0xF8) !=TW_SR_SLA_ACK)	return READ;
		return WRITE;
	}


}


/* Slave receiver function return value of incoming buffer and send an ACK sequence*/ 
unsigned char i2c_read_byte_slave_block()
{
	unsigned char temp = TWDR;
	TWCR = (1<<TWINT) |(1<<TWEN) |(1<<TWEA);
	i2c_wait();

	if((TWSR&0xF8) !=TW_SR_DATA_ACK) //data received, ACK returned 
	{
		i2c_error(TW_SR_DATA_ACK,(TWSR&0xF8));
	}

	return temp;


}

void i2c_write_byte_slave_block(unsigned char arg)
{
	TWDR = arg;

	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);;
	i2c_wait();



	if(!((TWSR&0xF8) == TW_ST_DATA_ACK || (TWSR&0xF8) == TW_ST_DATA_NACK) )
	{
		i2c_error(TW_ST_DATA_ACK,(TWSR&0xF8));
	}
	

}






/*Internal function, do not use! */
void i2c_wait()
{
	while((TWCR &(1<<TWINT)) == 0);
}

void i2c_error(unsigned char arg,unsigned char arg2)
{
			DDRC|=(1<<2);
			PORTC|=(1<<2);
			//uartSendByte(42);
}
	
