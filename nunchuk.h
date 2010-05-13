#ifndef NUNCHUK_H
#define	NUNCHUK_H


typedef struct 
{
	unsigned char stick_x;
	unsigned char stick_y;
	unsigned char acc_x;
	unsigned char acc_y;
	unsigned char acc_z;
	unsigned char button_Z;
	unsigned char button_C;
} nunchuk;


void nunchuk_init(void);
void nunchuk_refresh(nunchuk *);


#endif
