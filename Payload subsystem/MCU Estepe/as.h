#ifndef AS_H
#define AS_H

#include "C:\Users\Felipe\Desktop\AES_microchip\Payload_Subsystem\user.h"
#include "C:\Users\Felipe\Desktop\AES_microchip\Payload_Subsystem\fs.h"
#include "C:\Users\Felipe\Desktop\AES_microchip\Payload_Subsystem\bb.h"

int EPCS_device = 0;
int RPD_file_size = 0;

/*////////////////////*/
/* Global Definitions */
/*////////////////////*/

#define CHECK_EVERY_X_BYTE	10240
#define INIT_CYCLE			200



/*///////////////////////*/
/* AS Instruction Set    */
/*///////////////////////*/
#define AS_WRITE_ENABLE				0x06
#define AS_WRITE_DISABLE			0x04
#define AS_READ_STATUS	    		0x05
#define AS_WRITE_STATUS	    		0x01
#define AS_READ_BYTES   			0x03
#define AS_FAST_READ_BYTES  		0x0B
#define AS_PAGE_PROGRAM				0x02
#define AS_ERASE_SECTOR				0xD8
#define AS_ERASE_BULK				0xC7
#define AS_READ_SILICON_ID			0xAB
#define AS_CHECK_SILICON_ID			0x9F
#define AS_ENABLE_SECTOR			0x00

/*///////////////////////*/
/* Silicon ID for EPCS   */
/*///////////////////////*/
#define EPCS1_ID	0x10
#define EPCS4_ID	0x12
#define EPCS16_ID	0x14
#define EPCS64_ID	0x16
#define EPCS128_ID	0x18


/*///////////////////////*/
/* EPCS device			 */
/*///////////////////////*/
#define EPCS1		1
#define EPCS4		4
#define EPCS16		16
#define EPCS64		64
#define EPCS128		128

#define DEV_READBACK   0xFF //Special bypass indicator during EPCS data readback


/*///////////////////////*/
/* Functions Prototyping */
/*///////////////////////*/

int as_program(int epcsDensity );
int as_program_start(void);
int as_program_done(void);
int as_bulk_erase( void );
int as_prog(void);
int as_read(int);
int as_readback();
int as_silicon_id(int, int);
int as_program_byte_lsb( int one_byte  );
int as_read_byte_lsb(int *one_byte );
int as_program_byte_msb(int one_byte);
int as_read_byte_msb(int *one_byte );
void as_lsb_to_msb( int *in_byte, int *out_byte);


/********************************************************************************/
/*	Name:			as_program  												*/
/*																				*/
/*	Parameters:		FILE* finputid												*/
/*					- programming file pointer.									*/
/*																				*/
/*	Return Value:	Error Code													*/
/*																				*/
/*	Descriptions:	Get programming file size, parse through every single byte	*/
/*					and dump to parallel port.									*/
/*																				*/
/*					FPGA access to the EPCS is disable when the programming 	*/
/*					starts.														*/
/*																				*/
/*																				*/
/********************************************************************************/
int as_program(int epcsDensity )
{
	int status = 0;
	int file_id = 0;
	long int file_size = 0;

	restart_wdt();  //reset do WDT para continuar execução normal

	/* Disable FPGA access to EPCS */
	status = as_program_start();
	if ( status != CB_OK )
		return status;

	restart_wdt();  //reset do WDT para continuar execução normal

	status = as_silicon_id(file_size, epcsDensity);
	if ( status != CB_OK )
		return status;

	restart_wdt();  //reset do WDT para continuar execução normal

	/* EPCS Bulk Erase */
	status = as_bulk_erase( );
	if ( status != CB_OK )
		return status;

	restart_wdt();  //reset do WDT para continuar execução normal

	/* Start EPCS Programming */
	status = as_prog();
	if ( status != CB_OK )
		return status;

	restart_wdt();  //reset do WDT para continuar execução normal

	/* Enable FPGA access to EPCS */
	status = as_program_done();
	if ( status != CB_OK )
		return status;

	return CB_OK;

}


int as_program_start(void)
{
	int status = 0;

	// Drive NCONFIG to reset FPGA before programming EPCS
	status = bb_write( NCONFIG, 0 );
	if ( status != CB_OK )
		return status;

	// Drive NCE to disable FPGA from accessing EPCS
	status = bb_write( NCE, 1 );
	if ( status != CB_OK )
		return status;

	// Drive NCS to high when not acessing EPCS
	status = bb_write( NCS, 1 );
	if ( status != CB_OK )
		return status;

	restart_wdt();  //reset do WDT para continuar execução normal

	return CB_OK;
}
/********************************************************************************/
/*	Name:			as_read     												*/
/*																				*/
/*	Parameters:		FILE* finputid												*/
/*					- programming file pointer.									*/
/*																				*/
/*	Return Value:	Error Code													*/
/*																				*/
/*	Descriptions:	Get EPCS data and save in a file							*/
/*																				*/
/*																				*/
/*					FPGA access to the EPCS is disable when the reading     	*/
/*					starts.														*/
/*																				*/
/*																				*/
/********************************************************************************/
int as_read(int epcsDensity )
{
	int status = 0;
	int file_id = 0;

	/* Disable FPGA access to EPCS */
	status = as_program_start();
	if ( status != CB_OK )
		return status;


	/* Read EPCS silicon ID */
	status = as_silicon_id(DEV_READBACK, epcsDensity);
	if ( status != CB_OK )
		return status;
	


	/* Start EPCS Readback */
	status = as_readback();
	if ( status != CB_OK )
		return status;
	

	
	/* Enable FPGA access to EPCS */
	status = as_program_done();
	if ( status != CB_OK )
		return status;	


	return CB_OK;
	
}
int as_readback()
{
	//=========== Readback Program command Start=========//
	int		status;
	int32     i, w;
	int     read_byte;
	int32 j = 0;
		
		status = bb_write( NCS, 0 );
		if ( status != CB_OK )
			return status;
	
		status = as_program_byte_msb( AS_READ_BYTES );
		if ( status != CB_OK )
			return status;		
	
		
		status = as_program_byte_msb(0x00);
		status = as_program_byte_msb(0x00);
		status = as_program_byte_msb(0x00);	
		for(j=0; j<ADDR_MEM;j++){
			for(i=0; i<256; i++)
			{
				status = as_read_byte_lsb(&read_byte);
				if ( status != CB_OK )
					return status;
	
				DATA_PAGE_READ[i] = read_byte;
	
			}
			if(j == (ADDR_MEM-5)){
				w=0;
			}	
		}
		status = bb_write( NCS, 0 );
		if ( status != CB_OK )
			return status;

	return CB_OK;
}

int as_program_done(void)
{
	int		status;

	// Drive NCE to enable FPGA
	status = bb_write( NCE, 0 );
	if ( status != CB_OK )
		return status;

	// Drive NCONFIG from low to high to reset FPGA
	status = bb_write( NCONFIG, 1 );
	if ( status != CB_OK )
		return status;

	// Drive NCS to high when not acessing EPCS
	status = bb_write( NCS, 1 );
	if ( status != CB_OK )
		return status;

	restart_wdt();  //reset do WDT para continuar execução normal

	return CB_OK;
}

/********************************************************************************/
/*	Name:			as_prog														*/
/*																				*/
/*	Parameters:		int file_size					 							*/
/*					- file size to check for the correct programming file.		*/
/*					int file_id					 								*/
/*					- to refer to the RPD file.									*/
/*																				*/
/*	Return Value:	status.														*/
/*																				*/
/*	Descriptions:	program the data in the EPCS								*/
/*																				*/
/********************************************************************************/
int as_prog(){
	int			page = 0;
	int         one_byte = 0;
	int32         EPCS_Address =0;
	int         StatusReg =0;
	int32			i,j,w;		//vira int32 para suportar endereçamento
	int			status = 0;
//	int         bal_byte = 0; foi para o arquivo user como varivel global
	int16         byte_per_page = 256;


	//recebe via CAN
	//bal_byte = file_size%256;

	//if(bal_byte) //if there is balance after divide, program the balance in the next page
	//{
	//	page++;
	//}


	//=========== Page Program command Start=========//
	status = bb_write( NCS, 0 );
	if ( status != CB_OK )
		return status;

	status = as_program_byte_msb( AS_WRITE_ENABLE );
	if ( status != CB_OK )
		return status;

	status = bb_write( NCS, 1 );
	if ( status != CB_OK )
		return status;

	restart_wdt();  //reset do WDT para continuar execução normal

	i=0;
	//for(i=0; i<page; i++ ){   //! substituir por while(can_id == 0x10) indica paginação normal sem balance
	while(END_CONFIG != true){
		status = bb_write( NCS, 0 );
		if ( status != CB_OK )
			return status;

		status = as_program_byte_msb( AS_WRITE_ENABLE );
		if ( status != CB_OK )
			return status;

		status = bb_write( NCS, 1 );
		if ( status != CB_OK )
			return status;

		status = bb_write( NCS, 0 );
		if ( status != CB_OK )
			return status;

		status = as_program_byte_msb( AS_PAGE_PROGRAM );
		if ( status != CB_OK )
			return status;

		EPCS_Address = i*256;

		

	

		if(BUFFER_READY == true){
			BUFFER_READY = false;

		}
		//epera buffer encher via interrupção (tem que estar descriptografado)
		fs_read();
			
		restart_wdt();  //reset do WDT para continuar execução normal
					
		if(END_CONFIG == false){
			status = as_program_byte_msb( ((EPCS_Address & 0xFF0000)>>16));
			status = as_program_byte_msb( ((EPCS_Address & 0x00FF00)>>8) );
			status = as_program_byte_msb( EPCS_Address & 0xFF);
		
			restart_wdt();  //reset do WDT para continuar execução normal

			if(BAL_ACTIVE == true){	//if the last page has has been truncated less than 256
				byte_per_page = BAL_BYTE;				//retirar para fora do laço principal para trabalhar na ultima pagina
			}
			for(j=0; j<byte_per_page; j++){
				restart_wdt();  //reset do WDT para continuar execução normal
				status = as_program_byte_lsb( DATA_PAGE[j] ); //-one_byte -> buffer[j]
				if ( status != CB_OK )
					return status;
			}
				
			status = bb_write( NCS, 1 );
			if ( status != CB_OK )
				return status;
	
			//Program in proress
			status = bb_write( NCS, 0 );
			if ( status != CB_OK )
				return status;
	
			restart_wdt();  //reset do WDT para continuar execução normal
	
			status = as_program_byte_msb( AS_READ_STATUS );
			if ( status != CB_OK )
				return status;
	
	
			status = as_read_byte_msb(&StatusReg);
			if ( status != CB_OK )
				return status;
	
	
			while((StatusReg & 0x01))
			{
				restart_wdt();  //reset do WDT para continuar execução normal
				status = as_read_byte_msb(&StatusReg);
				if ( status != CB_OK )
					return status;
			}
	
	
			status = bb_write( NCS, 1 );
			if ( status != CB_OK )
				return status;
	
			restart_wdt();  //reset do WDT para continuar execução normal
	
	
			//Program End
			i++;
		}
	} 
	ADDR_MEM = i;
	FPGA_PACKET_BLOCKS = 0;
	BUFFER_READY = false; 
	END_CONFIG = false;
	PACKET_ARRIVE = false; 
	BAL_BYTE = 0;
	BAL_ACTIVE = false;
	status = fs_write(RESP_ACK_RECEIVE_DATA);
	return CB_OK;
}





/********************************************************************************/
/*	Name:			as_bulk_erase												*/
/*																				*/
/*	Parameters:		int file_size					 							*/
/*					- file size to check for the correct programming file.		*/
/*					int file_id					 								*/
/*					- to refer to the RPD file.									*/
/*																				*/
/*	Return Value:	status.														*/
/*																				*/
/*	Descriptions:	program the data in the EPCS								*/
/*																				*/
/********************************************************************************/
int as_bulk_erase( void )
{
	int status =0;
	int StatusReg =0;

	//=========== Bulk erase command Start ===========//
	status = bb_write( NCS, 0 );
	if ( status != CB_OK )
		return status;
	
	restart_wdt();  //reset do WDT para continuar execução normal
	status = as_program_byte_msb( AS_WRITE_ENABLE );
	if ( status != CB_OK )
		return status;
	
	status = bb_write( NCS, 1 );
	if ( status != CB_OK )
		return status;


	status = bb_write( NCS, 0 );
	if ( status != CB_OK )
		return status;
	restart_wdt();  //reset do WDT para continuar execução normal
	status = as_program_byte_msb( AS_ERASE_BULK );
	if ( status != CB_OK )
		return status;

	status = bb_write( NCS, 1 );
	if ( status != CB_OK )
		return status;

	
	status = bb_write( NCS, 0 );
	if ( status != CB_OK )
		return status;

	restart_wdt();  //reset do WDT para continuar execução normal
	status = as_program_byte_msb( AS_READ_STATUS );
	if ( status != CB_OK )
		return status;

	restart_wdt();  //reset do WDT para continuar execução normal
	status = as_read_byte_msb(&StatusReg);
	if ( status != CB_OK )
		return status;

	restart_wdt();  //reset do WDT para continuar execução normal
	while((StatusReg & 0x01))	//Keep on polling if the WIP is high
	{
		restart_wdt();  //reset do WDT para continuar execução normal
		status = as_read_byte_msb(&StatusReg);
		if ( status != CB_OK )
			return status;
	}

	status = bb_write( NCS, 1 );
	if ( status != CB_OK )
		return status;


	//=========== Bulk erase command End ============//
	return CB_OK;

}

/********************************************************************************/
/*	Name:			as_silicon_id												*/
/*																				*/
/*	Parameters:		int file_size					 							*/
/*					- file size to check for the correct programming file.		*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	check silicon id to determine:								*/
/*					EPCS devices.												*/
/*					RPD file size.												*/
/*																				*/
/********************************************************************************/
int as_silicon_id(int file_size, int epcsDensity)
{
	//=========== Read silicon id command Start=========//
	int	status;
	int silicon_ID = 0xFF;

		status = bb_write( NCS, 0 );
		if ( status != CB_OK )
			return status;

		if (epcsDensity != 128)		//for EPCS1, EPCS4, EPCS16, EPCS64
		{
			status = as_program_byte_msb( AS_READ_SILICON_ID );
			if ( status != CB_OK )
				return status;

			status = as_program_byte_msb(0x00);		//3 Dummy bytes
			status = as_program_byte_msb(0x00);
			status = as_program_byte_msb(0x00);
		}
		else						// for EPCS128
		{
			status = as_program_byte_msb( AS_CHECK_SILICON_ID );
			if ( status != CB_OK )
				return status;

			status = as_program_byte_msb(0x00);		//2 Dummy bytes
			status = as_program_byte_msb(0x00);
		}
		restart_wdt();  //reset do WDT para continuar execução normal
		// read silicon byte from the EPCS
		status = as_read_byte_msb(&silicon_ID);
			if ( status != CB_OK )
				return status;
		restart_wdt();  //reset do WDT para continuar execução normal
		while(silicon_ID == 0x00){
			restart_wdt();  //reset do WDT para continuar execução normal
			status = as_read_byte_msb(&silicon_ID);
			if ( status != CB_OK )
				return status;
		}		

		// determine the required RPD file size and EPCS devices
		if(silicon_ID == EPCS1_ID)
		{
			EPCS_device = EPCS1;
		}
		else if(silicon_ID == EPCS4_ID)
		{
			EPCS_device = EPCS4;
		}
		else if(silicon_ID == EPCS16_ID)
		{
			EPCS_device = EPCS16;
		}
		else if(silicon_ID == EPCS64_ID)
		{
			EPCS_device = EPCS64;
		}
		else if(silicon_ID == EPCS128_ID)
		{
			EPCS_device = EPCS128;
		}
		else
		{
			status = CB_AS_UNSUPPORTED_DEVICE;
			return status;
		}

		status = bb_write( NCS, 1 );
		if ( status != CB_OK )
			return status;

	return CB_OK;
}

/********************************************************************************/
/*	Name:			as_program_byte_lsb											*/
/*																				*/
/*	Parameters:		int one_byte					 							*/
/*					- The byte to dump.											*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	Dump to parallel port bit by bit, from least significant	*/
/*					bit to most significant bit. A positive edge clock pulse	*/
/*					is also asserted.											*/
/*																				*/
/********************************************************************************/
int as_program_byte_lsb( int one_byte )
{
	int	bit_t = 0;
	int i = 0;
	int status = 0;

	// write from LSB to MSB
	for ( i = 0; i < 8; i++ )
	{
		bit_t = one_byte >> i;
		bit_t = bit_t & 0x1;

		// Dump to DATA0 and insert a positive edge pulse at the same time
		status = bb_write( DCLK, 0 );
		if ( status != CB_OK )
			return status;
		//status = bb_write( DATA0, bit );
		status = bb_write( ASDI, bit_t );
		if ( status != CB_OK )
			return status;
		status = bb_write( DCLK, 1 );
		if ( status != CB_OK )
			return status;
	}


	return CB_OK;
}



/********************************************************************************/
/*	Name:			as_program_byte_msb											*/
/*																				*/
/*	Parameters:		int one_byte					 							*/
/*					- The byte to dump.											*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	Convert MSB to LSB and Dump to parallel port bit by bit,	*/
/*					from most significant bit to least significant bit.			*/
/*					A positive edge clock pulse	is also asserted.				*/
/*																				*/
/********************************************************************************/
int as_program_byte_msb( int one_byte )
{
	int status = 0;
	int data_byte = 0;

	//Convert MSB to LSB before programming
	as_lsb_to_msb(&one_byte, &data_byte);

	//After conversion, MSB will goes out first
	status = as_program_byte_lsb(data_byte);

	return CB_OK;
}


/********************************************************************************/
/*	Name:			as_read_byte_lsb											*/
/*																				*/
/*	Parameters:		int one_byte					 							*/
/*					- The byte to read.											*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	read to parallel port bit by bit, from least significant    */
/*					bit to most significant bit. A positive edge clock pulse	*/
/*					is also asserted. (read during positive edge)				*/
/*																				*/
/********************************************************************************/
int as_read_byte_lsb( int *one_byte )
{
	int	bit_t = 0;
	int mask = 0x01;
	int i;
	int status = 0;

	*one_byte = 0;


	// read from from LSB to MSB
	for ( i = 0; i < 8; i++ )
	{
		// Dump to DATA0 and insert a positive edge pulse at the same time
		status = bb_write( DCLK, 0 );
		if ( status != CB_OK )
			return status;

		status = bb_write( DCLK, 1 );
		if ( status != CB_OK )
			return status;

		// Flush the positive clk before reading
		bit_t = bb_read( DATAOUT, bit_t );
		if ( status != CB_OK )
			return status;

		if (bit_t!=0) //if bit is true
			*one_byte |= (mask << i);
	}

	return CB_OK;

}



/********************************************************************************/
/*	Name:			as_read_byte_msb											*/
/*																				*/
/*	Parameters:		int one_byte					 							*/
/*					- The byte to read.											*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	read from parallel port bit by bit, from most significant	*/
/*					bit to least significant bit. A positive edge clock pulse	*/
/*					is also asserted. (read during positive edge)				*/
/*																				*/
/********************************************************************************/
int as_read_byte_msb( int *one_byte )
{
	int status = 0;
	int data_byte = 0;

	status = as_read_byte_lsb(&data_byte);
	if ( status != CB_OK )
			return status;


	//After conversion, MSB will come in first
	as_lsb_to_msb(&data_byte, one_byte);

	return CB_OK;

}


/********************************************************************************/
/*	Name:			as_lsb_to_msb												*/
/*																				*/
/*	Parameters:		int *in_byte					 							*/
/*					- The byte to convert.										*/
/*					int *out_byte												*/
/*					- The converted byte										*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	Convert LSB to MSB											*/
/*																				*/
/*																				*/
/*																				*/
/********************************************************************************/
void as_lsb_to_msb( int *in_byte, int *out_byte)
{
	int		mask;
	int		i;
	int     temp;

	*out_byte = 0x00;


	for ( i = 0; i < 8; i++ )
	{
		temp = *in_byte >> i;
		mask = 0x80 >> i;

		if(temp & 0x01)	//if lsb is set inbyte, set msb for outbyte
		{
			*out_byte |= mask;
		}

	}
}


//#endif
