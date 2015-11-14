#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "simon.h"

u16 KeySeq[128] = {0x00, 0x00, 0x00,0x8000 };

// Cross correlation for (255 bit) (x) with (255 bit) (y)). correlationResult will have the result
// passed by reference (VALIDATED))
void CrossCorrelation(u8* x,u8* y, int* correlationResult)
{
    int i,j, sum=0;
    for(i=0;i<CROSSCORRELATIOM_SIZE;i++)
    {
        sum=0;
        for(j=0;j<CROSSCORRELATIOM_SIZE;j++)
        {
            if(x[j]==y[(j+i)%CROSSCORRELATIOM_SIZE])
            {
                sum = sum + 1;
            }else{
                sum = sum - 1;
            }
            
        }
        correlationResult[i] = sum;
    }
}

// This function will generate and assign the m-sequence to a global array according to the initial seed
void GenerateLFSR(u8 poly,u8 stages, u8 seed, u8* seqArray)
{
    int i=0;
    // This variable will be added to the sequence in case there is a carry bit
    u8 carryBit = (1 << stages - 1);
    // This variable will generate the bit mask of (00011111) in case LFSR dimension was 5 and so forth.
    u8 initiBitSeq = seed;
    u32 tempR; // Hold in previous state value of an LFSR
    
    for(i;i<((int)floor((pow(2,stages))-1));i++)
    {
        tempR = initiBitSeq & poly;
        tempR = NumberOfSetBits(tempR);
        // Shift the sequence 1 bit to the right
        seqArray[i] = initiBitSeq & 0x01;
        initiBitSeq = initiBitSeq >> 1;
        // Check if we need to add carry bit to the left of initiBitSeq
        // If least significant bit is on, this mean we have an odd tab result
        // which lead to add a carry
        if(tempR & 0x00000001)
        {
            initiBitSeq = initiBitSeq | carryBit;
        }
        //printf("%d", seqArray[i]);
    }
    //printf("\n%d",i);
}

// Calculate number of 1's in a sequence
u32 NumberOfSetBits(int i)
{
     i = i - ((i >> 1) & 0x55555555);
     i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
     return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

// Left circular 16bit shift function by j bits
u16 CircularLeftShift(u16 word, u8 j)
{
    return (word << j) | (word >> (16 - j));
}

// Left circular 16bit shift function by j bits
u16 CircularRightShift(u16 word, u8 j)
{
    return (word >> j) | (word << (16 - j));
}

// Pass the 16bit width plain text and key for encryption
void SimonEncryption(u16 *plainText, u16 *cipherText)
{
    int i;
    u16 tmp;
    // Initialize registers values
    for(i=0;i<2;i++)
    {
        cipherText[i%2] = plainText[i];
    }
    
    // Start the 32 rounds of Simon encryption
    for(i=0;i<32;i++)
    {
        tmp = cipherText[0]; // ==> 1
        cipherText[0] = ((CircularLeftShift(tmp,1)&CircularLeftShift(tmp,8))^cipherText[1])^CircularLeftShift(tmp,2)^KeySeq[i]; // x[1]
        cipherText[1] = tmp; // x[0]
        //printf("\'%x\',%d,",cipherText[1], i);
    }
}

// Function to generate keys from initial seeds
void KeyGeneration(u8 *LFSRSeq, u16 *KeySeq)
{
    u16 tmp, i, I;
    
    // Key initialization
    for(i=4;i<128;i++)
    {
        tmp = (C ^ (u16)LFSRSeq[(i-4)%31]);
        I = CircularRightShift(KeySeq[(i-1)],3) ^ KeySeq[(i-3)]; // Identity map
        KeySeq[i] =  (KeySeq[i-4] ^ I) ^ (CircularRightShift(I,1)) ^ (C ^ (u16)LFSRSeq[(i-4)%31]);
    }
}
