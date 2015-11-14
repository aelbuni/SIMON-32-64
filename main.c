/* 
 * File:   main.c
 * Author: Abdurrahman Elbuni
 *
 * Created on February 20, 2015, 11:39 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "simon.h"

extern u16 KeySeq[128];

// Main entry
int main(int argc, char** argv) {
    
    u32 i, j, k, testMask;
    u16 registers[2];
    u32 initialSeed = 0x1F; // Initial seed declared as 11111 
    u32 initialSeed8 = 0x01; // Initial seed declared as 00000001
    
    // {LSB, 0, .... 0, MSB}
    u16 plainText[2] = {0xf5a5, 0x3f01};
    u8 LFSRSeq5[32] = {0};
    u8 LFSRSeq8[255] = {0};
    
    u32 Yi[255] = {0};
    u32 Si[255] = {0};
    u8 SiTestVector [32][255] = {0};
    u8 YiTestVector [32][255] = {0};
    u8 *SiPointer;
    u8 *YiPointer;
    
    // Store cross correlation result for 32x16 (Message x Cipher).
    int crossCorrelation[32][16][CROSSCORRELATIOM_SIZE] = {0};
    int crossUpperLower[32][16][2] = {0};
    int *correlationPointer;

    // Initialize key array
    printf("Initial 5 stages m-sequence:\n");
    GenerateLFSR(LFSR_5_POLY, 5, initialSeed, LFSRSeq5);
    printf("\n");
    
    // Circular shift test
    // printf("%x\n\n",CircularRightShift(0x8200,1));
    
    // ************ Part (1) *************
    // ****** Key Scheduling ******
    KeyGeneration(LFSRSeq5, KeySeq);
    
    for(i=0;i<128;i++)
    {
        printf("Key[%d] = [%x]\n",i,KeySeq[i]);
    }
    
    // ****** Encryption ******
    SimonEncryption(plainText, registers);
            
    printf("%x %x",registers[0],registers[1]);
    
    // ************ Part (2) *************
    // ***********************************
    
    printf("\nInitial 8 stages m-sequence:\n");
    GenerateLFSR(LFSR_8_POLY, 8, initialSeed8, LFSRSeq8);
    printf("\n");
    
    for(i=0;i<255;i++)
    {
        for(j=32;j>0;j--)
        {
            Yi[i] = Yi[i] << 1;
            Yi[i] = Yi[i] | LFSRSeq8[(i+j-1)%255];
        }
        // temporary variable to hold current 32 bit input (16bit Concatenation)
        plainText[0]=Yi[i]>>16;
        plainText[1]=Yi[i]&0x0000ffff;
        // Apply SIMON Encryption
        SimonEncryption(plainText, registers);
        // Cipher text stored inside 32bit cipher array (16bit Concatenation))
        Si[i] = (registers[0]<<16);
        Si[i] = (registers[1]|Si[i]);
    }
    
    // Extract the first 16bit of cipher and 32bit of input for cross correlation
    for(i=0;i<255;i++)
    {
        testMask = 0x00000001;
        for(j=0;j<1;j++)
        {
            YiTestVector[j][i] = ((Yi[i]&testMask)>0)?1:0;
            testMask = testMask >> 1;
        }
    }
    for(i=0;i<255;i++)
    {
        testMask = 0x80000000;
        for(j=0;j<32;j++)
        {
            SiTestVector[j][i] = ((Si[i]&testMask)>0)?1:0;
            testMask = testMask >> 1;
        }
    }

    for(i=0;i<32;i++)
    {
        for(j=0;j<1;j++)
        {
            YiPointer = &YiTestVector[j][0];
            SiPointer = &SiTestVector[i][0];
            correlationPointer = &crossCorrelation[j][i][0];
            CrossCorrelation(SiPointer,YiPointer,correlationPointer);
        }
    }
    
    for(i=0;i<32;i++)
    {
        for(j=0;j<1;j++)
        {
            for(k=0;k<CROSSCORRELATIOM_SIZE;k++){
                //printf("%d,",crossCorrelation[i][j]);
                if(crossUpperLower[j][i][0]<crossCorrelation[j][i][k])
                {
                    crossUpperLower[j][i][0] = crossCorrelation[j][i][k];
                }
                if(crossUpperLower[j][i][1]>crossCorrelation[j][i][k])
                {
                    crossUpperLower[j][i][1] = crossCorrelation[j][i][k];
                }
            }
           
            //Print results
            printf("[MessageIndex,CipherIndex]=[%d,%d]\n",j,i);
            printf("[Message,Cipher]=[");
            for(k=0;k<CROSSCORRELATIOM_SIZE;k++){
                printf("%d",YiTestVector[j][k]);
            }
            printf(",\n");
            for(k=0;k<CROSSCORRELATIOM_SIZE;k++){
                printf("%d",SiTestVector[i][k]);
            }
            printf("]\n");
            printf("[upper,lower]=[%d,%d]",crossUpperLower[j][i][0],crossUpperLower[j][i][1]);
            printf(")\n");
        }
    }
    for(i=0;i<255;i++)
    {
        printf("S[%d] = [%x]\n",i,Si[i]);
    }
    for(i=0;i<255;i++)
    {
        printf("A[%d] = [%x]\n",i,Yi[i]);
    }
    
    return (EXIT_SUCCESS);
}