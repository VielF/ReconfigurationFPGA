#include "C:\Users\Felipe\Desktop\AES_microchip\Payload_Subsystem\main.h"
#define CAN_USE_EXTENDED_ID FALSE
//#include <can-18F4580.c>
#include <AES.h>
#include <as.h>
#include <user.h>
#byte INTCON = getenv("SFR:INTCON") 
#bit INT0IE = INTCON.4

#int_EXT
void  EXT_isr(void){
  SLEEP_MODE_STATUS= 0;
}


#int_CANRX0
void  CANRX0_isr(void){
   //Recebe comandos e
   //recebe o bitstream criptografado via interrupção
   //dois pacotes de 64 bits
   restart_wdt();  //reset do WDT para continuar execução normal
   if(can_getd(can_rx_id, &can_packet_rx[0], can_rx_len, rxstat)){
      if(can_rx_id == 10){
        restart_wdt();  //reset do WDT para continuar execução normal
		FPGA_REQ_RECONFIG = true;
      }else{
	     restart_wdt();  //reset do WDT para continuar execução normal
         PACKET_ARRIVE = true;
      }
   }

}


void main(){
   int status;
   int silicon_ID = 0x00;
   int file_id = 0;
   long int file_size = 0;
   int         StatusReg =0;

   setup_adc_ports(NO_ANALOGS|VSS_VDD);
   setup_adc(ADC_OFF);
   setup_psp(PSP_DISABLED);
   setup_spi(SPI_SS_DISABLED);
   setup_timer_0(RTCC_INTERNAL);
   setup_timer_1(T1_DISABLED);
   setup_timer_2(T2_DISABLED,0,1);
   setup_timer_3(T3_DISABLED|T3_DIV_BY_1);
   setup_comparator(NC_NC_NC_NC);
   setup_vref(FALSE);
   enable_interrupts(INT_EXT);    //Abilita interrupção externa no pino B0
   enable_interrupts(INT_CANTX0); //Abilita interrupção da tranmissão no barramento CAN
   enable_interrupts(INT_CANRX0); //Abilita interrupção da recepção no barramento CAN
   enable_interrupts(GLOBAL);

   /////////////////////LOW POWER MODE/////////////////////////////////////////////////
   enable_interrupts(INT_EXT); //Habilita interrupção externa no pino B0
   enable_interrupts(GLOBAL);  //Habilita interrupções 
   ext_int_edge(L_TO_H);  //Habilte a interrupção somente no de nível alto
   while(SLEEP_MODE_STATUS==1){ //loop até interrupção extena em B0 
	     sleep();
   }


/////////////////VERIFY PROTECT AREAS OF MEMORY/////////////////////// 
	status = bb_write( NCS, 0 );
	status = bb_write( NCS, 1 );	
	status = as_program_start();
		
	restart_wdt();  //reset do WDT para continuar execução normal
	
	status = as_silicon_id(file_size, 16);
		
	restart_wdt();  //reset do WDT para continuar execução normal
		
	status = bb_write( NCS, 1 );
	status = bb_write( NCS, 0 );
	restart_wdt();  //reset do WDT para continuar execução normal
	status = as_program_byte_msb( AS_WRITE_ENABLE );
	restart_wdt();  //reset do WDT para continuar execução normal
	status = bb_write( NCS, 1 );
	
	status = bb_write( NCS, 1 );
	status = bb_write( NCS, 0 );
	
	restart_wdt();  //reset do WDT para continuar execução normal	
	status = as_program_byte_msb( AS_READ_STATUS );	
	restart_wdt();  //reset do WDT para continuar execução normal
	status = as_read_byte_msb(&StatusReg);
	while((StatusReg & 0x01)){
		restart_wdt();  //reset do WDT para continuar execução normal
		status = as_read_byte_msb(&StatusReg);	
	}
		
	if((StatusReg & 0x10) || (StatusReg & 0x08) || (StatusReg & 0x04)){
		status = bb_write( NCS, 1 );
		status = bb_write( NCS, 0 );
		restart_wdt();  //reset do WDT para continuar execução normal
		as_program_byte_msb(AS_WRITE_STATUS);
		restart_wdt();  //reset do WDT para continuar execução normal
		as_program_byte_msb(0x00);
		status = bb_write( NCS, 1 );
		status = bb_write( NCS, 1 );
		status = bb_write( NCS, 0 );
		restart_wdt();  //reset do WDT para continuar execução normal	
		status = as_program_byte_msb( AS_READ_STATUS );	
		restart_wdt();  //reset do WDT para continuar execução normal
		status = as_read_byte_msb(&StatusReg);
		while((StatusReg & 0x01)){
			restart_wdt();  //reset do WDT para continuar execução normal
			status = as_read_byte_msb(&StatusReg);	
		}
	}
	restart_wdt();  //reset do WDT para continuar execução normal
	status = bb_write( NCS, 1 );
	status = as_program_done();
	restart_wdt();  //reset do WDT para continuar execução normal
		
/////////////////////////////////////////////////////////////////////////////////////
   	can_init(); //incia configuração da inteface CAN
	setup_wdt(WDT_ON);
	while(1){
      restart_wdt();  //reset do WDT para continuar execução normal
      if(FPGA_REQ_RECONFIG == true){
        //Identifica a REQ de reconfiguração da estação base e inicia a mesma
        FPGA_REQ_RECONFIG = false;
		restart_wdt();  //reset do WDT para continuar execução normal
        status = as_program(16);
		FPGA_PACKET_BLOCKS = 0;
		BUFFER_READY = false; 
		END_CONFIG = false;
	    PACKET_ARRIVE = false; 
		BAL_BYTE = 0;
		BAL_ACTIVE = false;
	    output_low(NCE);
		output_low(NCS);
        //as_read(16);
      }
   }




}
