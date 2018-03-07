/**
    Implementation of SHA-1 as described here:
    http://tools.ietf.org/html/rfc1319
*/

#include "SHA1.h"
#include <string.h>

#define F0(B,C,D) ((B & C) | ((~B) & D))
#define F1(B,C,D) (B ^ C ^ D)
#define F2(B,C,D) ((B & C) | (B & D) | (C & D))
#define ROTL(W,N) (((W) << N) | ((W) >> (32-N)))
                        
#define K0 0x5A827999
#define K1 0x6ED9EBA1
#define K2 0x8F1BBCDC
#define K3 0xCA62C1D6


#define H0 0x67452301
#define H1 0xEFCDAB89
#define H2 0x98BADCFE
#define H3 0x10325476
#define H4 0xC3D2E1F0

#define MASK 0xF

#define W(s) ( w[s] = ROTL(w[((s) + 13) & MASK] ^ w[((s) + 8) & MASK] ^ w[((s) + 2) & MASK] ^ w[s],1))

#define R0(A,B,C,D,E,T) E += ROTL(A, 5) + F0(B, C, D) + w[T] + K0; \
                        B = ROTL(B,30);
#define R1(A,B,C,D,E,T) E += ROTL(A, 5) + F0(B, C, D) + W(T & MASK) + K0; \
                        B = ROTL(B,30); 
#define R2(A,B,C,D,E,T) E += ROTL(A, 5) + F1(B, C, D) + W(T & MASK) + K1; \
                        B = ROTL(B,30); 
#define R3(A,B,C,D,E,T) E += ROTL(A, 5) + F2(B, C, D) + W(T & MASK) + K2; \
                        B = ROTL(B,30); 
#define R4(A,B,C,D,E,T) E += ROTL(A, 5) + F1(B, C, D) + W(T & MASK) + K3; \
                        B = ROTL(B,30); 


static uint32_t revWord(const uint32_t w)
{
#ifdef __CC_ARM
    return __rev(w);
#else
    return (w >> 24)
         | ((w & 0x00FF0000) >> 8)
         | ((w & 0x0000FF00) << 8)
         | ((w & 0x000000FF) << 24);
#endif
} 
                        
SHA1::SHA1():
HashAlgorithm(),
h0(H0),
h1(H1),
h2(H2),
h3(H3),
h4(H4),
totalBufferLength(0),
buffer(),
bufferLength(0)
{
}

uint8_t SHA1::outputSize() const
{
    return 20;
}

void SHA1::update(uint8_t *data, uint32_t length)
{
    if((int)length < 64-bufferLength)
    {
        memcpy(&buffer[bufferLength], data, length);
        bufferLength += length;
        totalBufferLength += length;
        return;
    }
    int offset = 64-bufferLength;
    memcpy(&buffer[bufferLength], data, offset);
    computeBlock(&h0,&h1,&h2,&h3,&h4, buffer);
    while(length-offset > 64)
    {
        memcpy(buffer, &data[offset], 64);
        computeBlock(&h0,&h1,&h2,&h3,&h4, buffer);
        offset += 64;
    }
    if(offset > (int)length)
        offset -= 64;
    bufferLength = length - offset;
    memcpy(buffer, &data[offset], bufferLength);
    totalBufferLength += length;
}

void SHA1::finalize(uint8_t *hash)
{
    uint32_t *hash2 = (uint32_t*)hash;
    uint16_t padding;
    if(totalBufferLength % 64 < 56)
        padding = 56 - (totalBufferLength % 64);
    else
        padding = 56 + (64 - (totalBufferLength % 64));
        
    buffer[bufferLength++] = 0x80;
    padding--;
    if(padding+bufferLength == 56)
        memset(&buffer[bufferLength], 0, padding);
    else
    {
        memset(&buffer[bufferLength], 0, 64-bufferLength);
        computeBlock(&h0,&h1,&h2,&h3,&h4, buffer);
        memset(buffer, 0, 56);
    }
    
    uint64_t lengthBit = totalBufferLength << 3;
    uint32_t lengthBitLow = lengthBit;
    uint32_t lengthBitHigh = lengthBit >> 32;
    lengthBitLow = revWord(lengthBitLow);
    lengthBitHigh = revWord(lengthBitHigh);
    memcpy(&buffer[56], &lengthBitHigh, 4);
    memcpy(&buffer[60], &lengthBitLow, 4);
    computeBlock(&h0,&h1,&h2,&h3,&h4, buffer);
    
    hash2[0] = revWord(h0);
    hash2[1] = revWord(h1);
    hash2[2] = revWord(h2);
    hash2[3] = revWord(h3);
    hash2[4] = revWord(h4);
    
    // reset state
    h0 = H0;
    h1 = H1;
    h2 = H2;
    h3 = H3;
    h4 = H4;
    totalBufferLength = 0;
    bufferLength = 0;
}


void SHA1::computeHash(uint8_t *hash, uint8_t *data, uint32_t length)
{
    uint32_t *hash2 = (uint32_t*)hash;
    uint64_t lengthBit = length << 3;
    uint32_t padding;
    if(length % 64 < 56)
        padding = 56 - (length % 64);
    else
        padding = 56 + (64 - (length % 64));
        
    uint32_t h0 = H0, h1 = H1, h2 = H2, h3 = H3, h4 = H4;
    while(length >= 64)
    {
        computeBlock(&h0,&h1,&h2,&h3,&h4, data);
        length -= 64;
        data += 64;
    }
   
    uint8_t buffer[64];
    memcpy(buffer, data, length);
    buffer[length++] = 0x80;
    padding--;
    if(padding+length+8 == 64)
        memset(&buffer[length], 0, padding);
    else
    {
        memset(&buffer[length], 0, 64-length);
        computeBlock(&h0,&h1,&h2,&h3,&h4, buffer);
        memset(buffer, 0, 56);
    }

    uint32_t lengthBitLow = lengthBit;
    uint32_t lengthBitHigh = lengthBit >> 32;
    lengthBitLow = revWord(lengthBitLow);
    lengthBitHigh = revWord(lengthBitHigh);
    memcpy(&buffer[60], &lengthBitLow, 4);
    memcpy(&buffer[56], &lengthBitHigh, 4);
    
    computeBlock(&h0,&h1,&h2,&h3,&h4, buffer);

    hash2[0] = revWord(h0);
    hash2[1] = revWord(h1);
    hash2[2] = revWord(h2);
    hash2[3] = revWord(h3);
    hash2[4] = revWord(h4);
}

void SHA1::computeBlock(uint32_t *h02, uint32_t *h12, uint32_t *h22, uint32_t *h32, uint32_t *h42, uint8_t *buffer)
{
    uint32_t *buffer2 = (uint32_t*)buffer;
    uint32_t w[16];

    for(int t = 0; t < 16; ++t)
        w[t] = revWord(buffer2[t]);
    
    uint32_t a = *h02, b = *h12, c = *h22, d = *h32, e = *h42;
    
    R0(a,b,c,d,e, 0) R0(e,a,b,c,d, 1) R0(d,e,a,b,c, 2) R0(c,d,e,a,b, 3)
    R0(b,c,d,e,a, 4) R0(a,b,c,d,e, 5) R0(e,a,b,c,d, 6) R0(d,e,a,b,c, 7)
    R0(c,d,e,a,b, 8) R0(b,c,d,e,a, 9) R0(a,b,c,d,e,10) R0(e,a,b,c,d,11)
    R0(d,e,a,b,c,12) R0(c,d,e,a,b,13) R0(b,c,d,e,a,14) R0(a,b,c,d,e,15)
    R1(e,a,b,c,d,16) R1(d,e,a,b,c,17) R1(c,d,e,a,b,18) R1(b,c,d,e,a,19)    
    
    
    R2(a,b,c,d,e,20) R2(e,a,b,c,d,21) R2(d,e,a,b,c,22) R2(c,d,e,a,b,23)
    R2(b,c,d,e,a,24) R2(a,b,c,d,e,25) R2(e,a,b,c,d,26) R2(d,e,a,b,c,27)
    R2(c,d,e,a,b,28) R2(b,c,d,e,a,29) R2(a,b,c,d,e,30) R2(e,a,b,c,d,31)
    R2(d,e,a,b,c,32) R2(c,d,e,a,b,33) R2(b,c,d,e,a,34) R2(a,b,c,d,e,35)
    R2(e,a,b,c,d,36) R2(d,e,a,b,c,37) R2(c,d,e,a,b,38) R2(b,c,d,e,a,39)    
    
    R3(a,b,c,d,e,40) R3(e,a,b,c,d,41) R3(d,e,a,b,c,42) R3(c,d,e,a,b,43)
    R3(b,c,d,e,a,44) R3(a,b,c,d,e,45) R3(e,a,b,c,d,46) R3(d,e,a,b,c,47)
    R3(c,d,e,a,b,48) R3(b,c,d,e,a,49) R3(a,b,c,d,e,50) R3(e,a,b,c,d,51)
    R3(d,e,a,b,c,52) R3(c,d,e,a,b,53) R3(b,c,d,e,a,54) R3(a,b,c,d,e,55)
    R3(e,a,b,c,d,56) R3(d,e,a,b,c,57) R3(c,d,e,a,b,58) R3(b,c,d,e,a,59)    
    
    
    R4(a,b,c,d,e,60) R4(e,a,b,c,d,61) R4(d,e,a,b,c,62) R4(c,d,e,a,b,63)
    R4(b,c,d,e,a,64) R4(a,b,c,d,e,65) R4(e,a,b,c,d,66) R4(d,e,a,b,c,67)
    R4(c,d,e,a,b,68) R4(b,c,d,e,a,69) R4(a,b,c,d,e,70) R4(e,a,b,c,d,71)
    R4(d,e,a,b,c,72) R4(c,d,e,a,b,73) R4(b,c,d,e,a,74) R4(a,b,c,d,e,75)
    R4(e,a,b,c,d,76) R4(d,e,a,b,c,77) R4(c,d,e,a,b,78) R4(b,c,d,e,a,79)    
        
    *h02 += a;
    *h12 += b;
    *h22 += c;
    *h32 += d;
    *h42 += e;
}
