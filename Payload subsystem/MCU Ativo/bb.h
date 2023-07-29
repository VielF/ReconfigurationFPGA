//#ifndef BB_H
//#define BB_H

#include <stdio.h>
#include "C:\Users\Felipe\Desktop\AES_microchip\Payload_Subsystem\user.h"

/*////////////////////*/
/* Global Definitions */
/*////////////////////*/

#define DCLK		PIN_C0  //modificado
#define NCONFIG		PIN_C6  //modificado
#define DATA0		0x40
#define CONF_DONE	PIN_C5  //modificado
#define NSTATUS		0x10
#define NCS			PIN_C1  //modificado
#define NCE			PIN_C3  //modificado
#define ASDI		PIN_B5  //modificado
#define DATAOUT		PIN_C4  //modificado
#define TCK			0x01
#define TMS			0x02
#define TDI			0x40
#define TDO			0x80

//#define DCLK		PIN_D0  //modificado
//#define NCONFIG		PIN_D1  //modificado
//#define DATA0		0x40
//#define CONF_DONE	PIN_C1  //modificado
//#define NSTATUS		0x10
//#define NCS			PIN_D3  //modificado
//#define NCE			PIN_D4  //modificado
//#define ASDI		PIN_D5  //modificado
//#define DATAOUT		PIN_C0  //modificado
//#define TCK			0x01
//#define TMS			0x02
//#define TDI			0x40
//#define TDO			0x80



/*///////////////////////*/
/* Functions Prototyping */
/*///////////////////////*/

int bb_read( int signal, int data );
int bb_write( int signal, int data);
//int bb_reset	( int );  //talvez tirar, serva para driver no windows


int bb_read( int signal, int data )
{
	int temp = 0;
	int	status = 0;
	if(input(signal)){
		data = 1;
	}else{
		data = 0;
	}

	return data;
}

int bb_write( int signal, int data ){
	int status = 0;

	/* AND signal bit with '0', then OR with [data] */
	if(data == 1){
		output_high(signal);
	}else{
		output_low(signal);
	}

	return CB_OK;
}



#endif
