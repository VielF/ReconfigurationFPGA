//#ifndef FS_H
//#define FS_H


#include <stdio.h>
#include "C:\Users\Felipe\Desktop\AES_microchip\Payload_Subsystem\user.h"
/*////////////////////*/
/* Global Definitions */
/*////////////////////*/

#define S_CUR				1 /* SEEK_CUR */
#define S_END				2 /* SEEK_END */
#define S_SET				0 /* SEEK_SET */


/*///////////////////////*/
/* Functions Prototyping */
/*///////////////////////*/

int fs_read();
int fs_write(int WRITING_TYPE);		//Srunner 4827



int fs_read(){
	int status = 0;
	int i=0;
	unsigned char data[16] = {0x00}; //temporário dos que armzena os dados para descriptografia AES
	unsigned char AESkey[16]; //Armzena a chave de descriptografia temporaria
	int lenght_buffer = 0;   //controla se o buffer contem 256 bits
	int writing_t = 0;
	int low_part = 0;
	fs_write(RESP_ACK_RECEIVE_DATA);

	while(BUFFER_READY == false && END_CONFIG == false){
		restart_wdt();  //reset do WDT para continuar execução normal
		if(PACKET_ARRIVE == true){
			//if(can_getd(can_rx_id, &can_packet_rx[0], can_rx_len, rxstat)){
				//controle para os dois pacotes de 64 bits que formam o pacote de 128 bits do AES
				//é zerado ao fim da descriptografia
				PACKET_ARRIVE = false;
				restart_wdt();  //reset do WDT para continuar execução normal
				if(FPGA_PACKET_BLOCKS < 2){
					FPGA_PACKET_BLOCKS++;
					if(FPGA_PACKET_BLOCKS == 2){
						low_part = 8;
					}else{
						low_part = 0;
					}
					for(i=0;i<8;i++){
						data[low_part+i] = (unsigned char)can_packet_rx[i];
					}
					restart_wdt();  //reset do WDT para continuar execução normal
					//Verifica se já foram dois pacotes de 64 bits e se o segundo é os 8 bits menos significaticos
					if(FPGA_PACKET_BLOCKS == 2 || rxstat.rtr == 1){
						for(i=0;i<16;i++){ //Copia a chave de descriptografia
								AESkey[i]=DECODE_KEY[i];
						}
						restart_wdt();  //reset do WDT para continuar execução normal
						AESDecode(data,AESkey); //Descriptografa
						restart_wdt();  //reset do WDT para continuar execução normal
						FPGA_PACKET_BLOCKS = 0;
						i=0;
						for(i=0; i<16;i++){
							DATA_PAGE[lenght_buffer+i] = (int)data[i]; //copia os dados criptografados para o buffer de paginação
						}
						restart_wdt();  //reset do WDT para continuar execução normal
						lenght_buffer+=16; //Adiciona mais 16 bytes ao vetor de paginação
						if(can_rx_id != 3 && can_rx_id != 4 && can_rx_id != 10 && rxstat.rtr != true ){
							BAL_BYTE = (int)can_rx_id;
							BAL_ACTIVE = true;
							BUFFER_READY = true;
						}
						if(rxstat.rtr == 1 && can_rx_id == 255){
							END_CONFIG = true;
							
						}
						restart_wdt();  //reset do WDT para continuar execução normal
						if(lenght_buffer == 256 || BAL_ACTIVE == true){
							//garante que o buffer possui os 256 bits da página
							BUFFER_READY = true;  //Na as_prog é respondido o recebimento dos pacotes após armazenamento dos atuais na memória
						}else{
							//responde aqui para reenviar os bytes restante para encher o buffer
							status = fs_write(RESP_ACK_RECEIVE_DATA);
						}
					}else{
						//responde aqui para reenviar os bytes restante para encher o buffer
						status = fs_write(RESP_ACK_RECEIVE_DATA);
					}
				}
			//}
		}
	}
	return CB_OK;

}


int fs_write(int WRITING_TYPE){
	int status = 0;
	int RESP_AUX_DATA = 0;
	restart_wdt();  //reset do WDT para continuar execução normal
	switch (WRITING_TYPE) {
		case RESP_ACK_RECEIVE_DATA :  status=can_putd(CAN_TX_ID_ACK, RESP_AUX_DATA, 1,tx_pri,tx_ext,1);
				 					  break;
		case DATA_EXPERIMENT :  status=can_putd(CAN_TX_ID_RES_EXP, EXPERIMENT_RESULT, 1,tx_pri,tx_ext,0);
				 				break;
	}
	if(status != 0xFF){
		return CB_OK;
	}else{
		return CB_FS_SEND_RESULT_ERROR;
	}


}

//#endif
