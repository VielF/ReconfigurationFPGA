
#define SYN             50   //SYN - sincronismo 50 = 0x32
#define SOH             1    //SOH - inicio cabe�alho 1 = 0x01
#define SOH_BAL         2    //SOH_BAL - inicio cabe�alho balanceamento 2 = 0x02
#define SOH_END         3    //SOH - inicio cabe�alho fim arquivo 3 = 0x03
#define DEFAULT_HEADER  16   //cabe�alho - 0x10 = 16(n pacotes)
#define DONE_HEADER     255  //cabe�alho - 0xFF = fim do arquivo
#define STX             2    //STX - inicio do payload 2 = 0x02
#define ETX             3    //ETX - fim payload 3 = 0x03
#define INIT_TRANSM     10   //INIT_TRANSM - indica o inicio da transmiss�o - 10 = 0x0A