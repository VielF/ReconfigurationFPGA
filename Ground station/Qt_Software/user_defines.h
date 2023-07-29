#ifndef USER_DEFINES_H
#define USER_DEFINES_H

#define SYN             50   //SYN - sincronismo 50 = 0x32
#define SOH             1    //SOH - inicio cabeçalho 1 = 0x01
#define SOH_BAL         2    //SOH - inicio cabeçalho balançeamento 2 = 0x02
#define SOH_END         3    //SOH - inicio cabeçalho fim arquivo 3 = 0x03
#define DEFAULT_HEADER  16   //cabeçalho - 0x10 = 16(n pacotes)
#define DONE_HEADER     255  //cabeçalho - 0xFF = fim do arquivo
#define STX             2    //STX - inicio do payload 2 = 0x02
#define ETX             3    //ETX - fim payload 3 = 0x03
#define INIT_TRANSM     10   //INIT_TRANSM - indica o inicio da transmissão - 10 = 0x0A

#endif // USER_DEFINES_H
