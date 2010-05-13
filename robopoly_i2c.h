#ifndef ROBOPOLY_I2C_H
#define	ROBOPOLY_I2C_H


enum ack
{
	NACK = 0,
	ACK
};


#define WRITE 0
#define READ 1


void i2c_init_master(void);
void i2c_init_slave(unsigned char);

//master
void i2c_send_start_block(void);
void i2c_send_restart_block(void);
void i2c_send_stop_block(void);
void i2c_send_byte_block(unsigned char);
void i2c_send_addr_block(unsigned char,unsigned char);

//Master
unsigned char i2c_read_byte_block(unsigned char);


/*Slave function return WRITE or READ (master want to write or read data)*/
unsigned char i2c_read_addr_slave_block(void);

//Slave receiver
unsigned char i2c_read_byte_slave_block(void);

//Slave transmitter
void i2c_write_byte_slave_block(unsigned char);

/* Internal function (do not use) */
void i2c_wait(void);
void i2c_error(unsigned char, unsigned char);

#endif
