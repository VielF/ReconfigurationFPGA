#include "C:\Users\Felipe\Desktop\AES_microchip\Comunication_Subsystem\main.h"
#define CAN_USE_EXTENDED_ID FALSE
#include <can-18F4580.c>
#include "C:\Users\Felipe\Desktop\AES_microchip\Comunication_Subsystem\user_defines.h"

struct rx_stat rxstat;
int32 can_rx_id;
int can_packet_rx[8];
int can_rx_len;
int1 can_data_ack=0;


#int_CANRX0
void  CANRX0_isr(void){

	//recebe pacote
	if(can_getd(can_rx_id, &can_packet_rx[0], can_rx_len, rxstat))
	//verifica se é resposta
	if(can_rx_id ==0x01){
		can_data_ack = 1;
	}/*else{
		//verifica se é resultado da simulação
		if(can_rx == 0x02){
			//manda dado
		}*/
}

void main(){
	int i = 0, pack_pos =0;
	unsigned char initi_transm = 0x00;
	unsigned char packet_uart[22] = {0x00};
	int can_flag_tx_L = 0, can_flag_tx_H = 0, can_flag_end_tx = 0, can_erro_trasmission =0, can_flag_tx_BAL_H = 0, can_flag_tx_BAL_L = 0;
	int can_packet_L[8] = {0x00};
	int can_packet_H[8] = {0x00}; 
   	int32 can_tx_id_L=3;
	int32 can_tx_id_H=4;
	int32 can_tx_id_end=255;
   	int1 tx_rtr=0, tx_rtr_end=1;
   	int1 tx_ext=0;
   	int tx_len=8;
   	int tx_pri=3, tx_pri_L=3, tx_pri_END = 2;
	int p=0;



   	setup_adc_ports(NO_ANALOGS|VSS_VDD);
   	setup_adc(ADC_OFF);
   	setup_psp(PSP_DISABLED);
   	setup_spi(SPI_SS_DISABLED);
   	setup_wdt(WDT_OFF);
   	setup_timer_0(RTCC_INTERNAL);
   	setup_timer_1(T1_DISABLED);
   	setup_timer_2(T2_DISABLED,0,1);
   	setup_comparator(NC_NC_NC_NC);
   	setup_vref(FALSE);
   	//enable_interrupts(INT_CANTX2);
   	//enable_interrupts(INT_CANTX1);
   	//enable_interrupts(INT_CANTX0);
   	//enable_interrupts(INT_CANRX1);
    enable_interrupts(INT_CANRX0);
   	enable_interrupts(GLOBAL);

	can_init();
	set_uart_speed(115200);
    output_high(PIN_D0);
	while(1){
		initi_transm = getch(); //recebe inicio da comunica��o
		if(initi_transm == INIT_TRANSM){ //verificca se � o iniciador da comunicação
			can_putd(INIT_TRANSM, 0,8,tx_pri,tx_ext,1); //envia REQ para inciar reconfiguração
			//while(can_data_ack!=0x01);
			printf("%x",0x10);
			can_data_ack = 0x00;
			while(packet_uart[3] != DONE_HEADER && packet_uart[2] != 3){ //fica repassa dados enquanto não for informado o fim do arquivo de configuração
				while(i<22){
					packet_uart[i] = getch();
					i++;
				}
				//verifica sincronismo, cabeçalho e iniciador de menssagem
				if(packet_uart[0] == SYN && packet_uart[1] == SYN && (packet_uart[2] == SOH || packet_uart[2] == SOH_BAL) && packet_uart[3] != DONE_HEADER){
					//cria��o dos doi pacotes
					i=0;
					if(packet_uart[3] != 16 && packet_uart[2] == 2){
						can_tx_id_L = (int32)packet_uart[3];
						tx_pri_L=1;
					}
					for(i=0;i<8;i++){
						can_packet_H[i]=(int)packet_uart[i+5];
						can_packet_L[i]=(int)packet_uart[i+13];
					}
					//trasmite os dois pacotes
					can_flag_tx_H=can_putd(can_tx_id_H, can_packet_H, tx_len,tx_pri,tx_ext,tx_rtr);
					while(can_data_ack!=0x01); //espera a resposta de pacote recebido
					can_data_ack = 0;

					can_flag_tx_L=can_putd(can_tx_id_L, can_packet_L, tx_len,tx_pri_L,tx_ext,tx_rtr);
					while(can_data_ack!=0x01); //espera a resposta de pacote recebido
					can_data_ack = 0;
					if(can_flag_tx_L == 0xFF || can_flag_tx_H == 0xFF){ //verifica se houve erros na trasmissão
						can_erro_trasmission = 1;
					}
					
					i=0;
					printf("%x",0x70); //responde para estação base que o pacote foi enviado com sucesso;
				}
				
				

			}
			
			//envia pacote terminado para identificar fim do arquivo de configuração
			can_flag_end_tx=can_putd(can_tx_id_end, can_packet_L, tx_len,tx_pri_END,tx_ext,tx_rtr_end);
			while(can_data_ack!=0x01);
			printf("%x",0x50); //responde para estação base que o pacote foi enviado com sucesso;
			i=0;
		}
	}
}
