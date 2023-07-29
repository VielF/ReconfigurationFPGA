#ifndef AES_H
#define AES_H

void AESEncode(unsigned char* block, unsigned char* key);
void AESDecode(unsigned char* block, unsigned char* key);
void AESCalcDecodeKey(unsigned char* key);


#endif // AES_H
