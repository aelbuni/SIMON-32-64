/* 
 * File:   main.h
 * Author: aelbuni
 *
 * Created on March 4, 2015, 11:11 AM
 */

#define N 16
#define T 4

#define u32 unsigned int
#define u16 unsigned short
#define u8 unsigned char

// x^5+x^4+x^2+x+1
#define LFSR_5_POLY 0x17
// x^8+x^7+x^2+x+1
#define LFSR_8_POLY 0x87

#define CROSSCORRELATIOM_SIZE 255

// Constants 
// C=> 2^16-4 (1111111111111100b))
#define C 0xFFFC

// Function prototypes section
void GenerateLFSR(u8 poly,u8 stages, u8 seed, u8* seqArray);
u32 NumberOfSetBits(int i);
u16 CircularLeftShift(u16 word, u8 j);
u16 CircularRightShift(u16 word, u8 j);
void SimonEncryption(u16 *plainText, u16 *cipherText);
void KeyGeneration(u8 *LFSRSeq, u16 *KeySeq);
void CrossCorrelation(u8* x,u8* y, int* correlationResult);

// ********** Key input is 4x16=64 bits
// ********** Testing input
//u16 KeySeq[128] = {0x0100, 0x0908, 0x1110, 0x1918 };
//u16 plainText[2] = {0x6877, 0x6565};
//u16 cipherText[2] = {0xe9bb, 0xc69b};

//u16 KeySeq[128] = {0x80, 0x00, 0x00, 0x00 };
//u16 plainText[2] = {0x6877, 0x6565};
//u16 cipherText[2] = {0xe9bb, 0xc69b};