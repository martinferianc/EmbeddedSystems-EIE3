#include "MD4.h"
#include <string.h>

static const uint32_t A = 0x67452301;
static const uint32_t B = 0xefcdab89;
static const uint32_t C = 0x98badcfe;
static const uint32_t D = 0x10325476;

#define F(X,Y,Z) ((X & Y) | ((~X) & Z))
#define G(X,Y,Z) ((X & Y) | (X & Z) | (Y & Z))
#define H(X,Y,Z) (X ^ Y ^ Z)


#define ROTL(W,N) (((W) << N) | ((W) >> (32-N)))

#define ROUND1(a,b,c,d,x,s) \
    a = ROTL(a + F(b,c,d) + x,s); 

#define ROUND2(a,b,c,d,x,s) \
    a = ROTL(a + G(b,c,d) + x + 0x5A827999,s); 

#define ROUND3(a,b,c,d,x,s) \
    a = ROTL(a + H(b,c,d) + x + 0x6ED9EBA1,s); 


    
MD4::MD4():
HashAlgorithm(),
a(A),
b(B),
c(C),
d(D),
totalBufferLength(0),
buffer(),
bufferLength(0)
{
}

uint8_t MD4::outputSize() const
{
    return 16;
}

void MD4::update(uint8_t *data, uint32_t length)
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
    computeRounds(&a, &b, &c, &d, buffer);
    while(length-offset > 64)
    {
        memcpy(buffer, &data[offset], 64);
        computeRounds(&a, &b, &c, &d, buffer);
        offset += 64;
    }
    if(offset > (int)length)
        offset -= 64;
    bufferLength = length - offset;
    memcpy(buffer, &data[offset], bufferLength);
    totalBufferLength += length;
}

void MD4::finalize(uint8_t *hash)
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
        computeRounds(&a, &b, &c, &d, buffer);
        memset(buffer, 0, 56);
    }
    uint64_t lengthBit = totalBufferLength << 3;
    uint32_t lengthBitLow = lengthBit;
    uint32_t lengthBitHigh = lengthBit >> 32;
    memcpy(&buffer[56], &lengthBitLow, 4);
    memcpy(&buffer[60], &lengthBitHigh, 4);
    computeRounds(&a, &b, &c, &d, buffer);

    hash2[0] = a;
    hash2[1] = b;
    hash2[2] = c;
    hash2[3] = d;
    // reset state
    a = A;
    b = B;
    c = C;
    d = D;
    totalBufferLength = 0;
    bufferLength = 0;
}


void MD4::computeHash(uint8_t *hash, uint8_t *data, uint32_t length)
{
    uint32_t *hash2 = (uint32_t*)hash;
    uint64_t lengthBit = length << 3;
    uint16_t padding;
    if(length % 64 < 56)
        padding = 56 - (length % 64);
    else
        padding = 56 + (64 - (length % 64));
        
    uint32_t a = A, b = B, c = C, d = D;
    while(length >= 64)
    {
        computeRounds(&a, &b, &c, &d, data);
        data += 64;
        length -= 64;
    }
    uint8_t buffer[64];
    memcpy(buffer, data, length);
    buffer[length++] = 0x80;
    padding--;
    if(padding+length == 56)
        memset(&buffer[length], 0, padding);
    else
    {
        memset(&buffer[length], 0, 64-length);
        computeRounds(&a, &b, &c, &d, data);
        memset(buffer, 0, 56);
    }

    uint32_t lengthBitLow = lengthBit;
    uint32_t lengthBitHigh = lengthBit >> 32;
    memcpy(&buffer[56], &lengthBitLow, 4);
    memcpy(&buffer[60], &lengthBitHigh, 4);
    
    computeRounds(&a, &b, &c, &d, buffer);
    
    hash2[0] = a;
    hash2[1] = b;
    hash2[2] = c;
    hash2[3] = d;
}

void MD4::computeRounds(uint32_t *a2, uint32_t *b2, uint32_t *c2, uint32_t *d2, uint8_t *buffer)
{
    uint32_t a = *a2, b = *b2, c = *c2, d = *d2;
    uint32_t tmpA = a, tmpB = b, tmpC = c, tmpD = d;

    uint32_t *x = (uint32_t*)buffer;
       
    // Round 1
    ROUND1(a,b,c,d,x[0],3);     ROUND1(d,a,b,c,x[1],7);     ROUND1(c,d,a,b,x[2],11);    ROUND1(b,c,d,a,x[3],19);
    ROUND1(a,b,c,d,x[4],3);     ROUND1(d,a,b,c,x[5],7);     ROUND1(c,d,a,b,x[6],11);    ROUND1(b,c,d,a,x[7],19);
    ROUND1(a,b,c,d,x[8],3);     ROUND1(d,a,b,c,x[9],7);     ROUND1(c,d,a,b,x[10],11);   ROUND1(b,c,d,a,x[11],19);
    ROUND1(a,b,c,d,x[12],3);    ROUND1(d,a,b,c,x[13],7);    ROUND1(c,d,a,b,x[14],11);   ROUND1(b,c,d,a,x[15],19);
    
    // Round 2      
    ROUND2(a,b,c,d,x[0],3);     ROUND2(d,a,b,c,x[4],5);     ROUND2(c,d,a,b,x[8],9);     ROUND2(b,c,d,a,x[12],13);
    ROUND2(a,b,c,d,x[1],3);     ROUND2(d,a,b,c,x[5],5);     ROUND2(c,d,a,b,x[9],9);     ROUND2(b,c,d,a,x[13],13);
    ROUND2(a,b,c,d,x[2],3);     ROUND2(d,a,b,c,x[6],5);     ROUND2(c,d,a,b,x[10],9);    ROUND2(b,c,d,a,x[14],13);
    ROUND2(a,b,c,d,x[3],3);     ROUND2(d,a,b,c,x[7],5);     ROUND2(c,d,a,b,x[11],9);    ROUND2(b,c,d,a,x[15],13);
    
    // Round 3      
    ROUND3(a,b,c,d,x[0],3);     ROUND3(d,a,b,c,x[8],9);     ROUND3(c,d,a,b,x[4],11);    ROUND3(b,c,d,a,x[12],15);
    ROUND3(a,b,c,d,x[2],3);     ROUND3(d,a,b,c,x[10],9);    ROUND3(c,d,a,b,x[6],11);    ROUND3(b,c,d,a,x[14],15);
    ROUND3(a,b,c,d,x[1],3);     ROUND3(d,a,b,c,x[9],9);     ROUND3(c,d,a,b,x[5],11);    ROUND3(b,c,d,a,x[13],15);
    ROUND3(a,b,c,d,x[3],3);     ROUND3(d,a,b,c,x[11],9);    ROUND3(c,d,a,b,x[7],11);    ROUND3(b,c,d,a,x[15],15);
     

    *a2 = a + tmpA;
    *b2 = b + tmpB;
    *c2 = c + tmpC;
    *d2 = d + tmpD;
}