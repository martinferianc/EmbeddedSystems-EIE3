/**
    Implementation of MD5 as described here:
    http://tools.ietf.org/html/rfc1321
*/

#include "MD5.h"
#include <string.h>

static const uint32_t A = 0x67452301;
static const uint32_t B = 0xefcdab89;
static const uint32_t C = 0x98badcfe;
static const uint32_t D = 0x10325476;


#define F(X,Y,Z) (((X) & (Y)) | ((~(X)) & (Z)))
#define G(X,Y,Z) (((X) & (Z)) | ((Y) & (~(Z))))
#define H(X,Y,Z) ((X) ^ (Y) ^ (Z))
#define I(X,Y,Z) ((Y) ^ ((X) | (~(Z))))

#define ROTL(W,N) (((W) << N) | ((W) >> (32-N)))

#define ROUND1(a,b,c,d,x,s,t) \
    a = ROTL(a + F(b,c,d) + x + t,s) + b; 

#define ROUND2(a,b,c,d,x,s,t) \
    a = ROTL(a + G(b,c,d) + x + t,s) + b; 

#define ROUND3(a,b,c,d,x,s,t) \
    a = ROTL(a + H(b,c,d) + x + t,s) + b; 

#define ROUND4(a,b,c,d,x,s,t) \
    a = ROTL(a + I(b,c,d) + x + t,s) + b; 


    
MD5::MD5():
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

uint8_t MD5::outputSize() const
{
    return 16;
}

void MD5::update(uint8_t *data, uint32_t length)
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

void MD5::finalize(uint8_t *hash)
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


void MD5::computeHash(uint8_t *hash, uint8_t *data, uint32_t length)
{
    uint32_t *hash2 = (uint32_t*)hash;
    uint64_t lengthBit = length << 3;
    uint32_t padding;
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



#ifdef __CC_ARM
__forceinline 
#endif
void MD5::computeRounds(uint32_t *a2, uint32_t *b2, uint32_t *c2, uint32_t *d2, uint8_t *buffer)
{
    uint32_t a = *a2, b = *b2, c = *c2, d = *d2;
    uint32_t tmpA = a, tmpB = b, tmpC = c, tmpD = d;

    uint32_t *x = (uint32_t*)buffer;
       
    // Round 1
    ROUND1(a,b,c,d,x[0],7,0xd76aa478);     ROUND1(d,a,b,c,x[1],12,0xe8c7b756);    ROUND1(c,d,a,b,x[2],17,0x242070db);    ROUND1(b,c,d,a,x[3],22,0xc1bdceee);
    ROUND1(a,b,c,d,x[4],7,0xf57c0faf);     ROUND1(d,a,b,c,x[5],12,0x4787c62a);    ROUND1(c,d,a,b,x[6],17,0xa8304613);    ROUND1(b,c,d,a,x[7],22,0xfd469501);
    ROUND1(a,b,c,d,x[8],7,0x698098d8);     ROUND1(d,a,b,c,x[9],12,0x8b44f7af);    ROUND1(c,d,a,b,x[10],17,0xffff5bb1);   ROUND1(b,c,d,a,x[11],22,0x895cd7be);
    ROUND1(a,b,c,d,x[12],7,0x6b901122);    ROUND1(d,a,b,c,x[13],12,0xfd987193);   ROUND1(c,d,a,b,x[14],17,0xa679438e);   ROUND1(b,c,d,a,x[15],22,0x49b40821);


    // Round 2      
    ROUND2(a,b,c,d,x[1],5,0xf61e2562);     ROUND2(d,a,b,c,x[6],9,0xc040b340);     ROUND2(c,d,a,b,x[11],14,0x265e5a51);   ROUND2(b,c,d,a,x[0],20,0xe9b6c7aa);
    ROUND2(a,b,c,d,x[5],5,0xd62f105d);     ROUND2(d,a,b,c,x[10],9,0x02441453);    ROUND2(c,d,a,b,x[15],14,0xd8a1e681);   ROUND2(b,c,d,a,x[4],20,0xe7d3fbc8);
    ROUND2(a,b,c,d,x[9],5,0x21e1cde6);     ROUND2(d,a,b,c,x[14],9,0xc33707d6);    ROUND2(c,d,a,b,x[3],14,0xf4d50d87);    ROUND2(b,c,d,a,x[8],20,0x455a14ed);
    ROUND2(a,b,c,d,x[13],5,0xa9e3e905);    ROUND2(d,a,b,c,x[2],9,0xfcefa3f8);     ROUND2(c,d,a,b,x[7],14,0x676f02d9);    ROUND2(b,c,d,a,x[12],20,0x8d2a4c8a);
    

    // Round 3      
    ROUND3(a,b,c,d,x[5],4,0xfffa3942);     ROUND3(d,a,b,c,x[8],11,0x8771f681);    ROUND3(c,d,a,b,x[11],16,0x6d9d6122);   ROUND3(b,c,d,a,x[14],23,0xfde5380c);
    ROUND3(a,b,c,d,x[1],4,0xa4beea44);     ROUND3(d,a,b,c,x[4],11,0x4bdecfa9);    ROUND3(c,d,a,b,x[7],16,0xf6bb4b60);    ROUND3(b,c,d,a,x[10],23,0xbebfbc70);
    ROUND3(a,b,c,d,x[13],4,0x289b7ec6);    ROUND3(d,a,b,c,x[0],11,0xeaa127fa);    ROUND3(c,d,a,b,x[3],16,0xd4ef3085);    ROUND3(b,c,d,a,x[6],23,0x04881d05);
    ROUND3(a,b,c,d,x[9],4,0xd9d4d039);     ROUND3(d,a,b,c,x[12],11,0xe6db99e5);   ROUND3(c,d,a,b,x[15],16,0x1fa27cf8);   ROUND3(b,c,d,a,x[2],23,0xc4ac5665);
 
 
    // Round 4
    ROUND4(a,b,c,d,x[0],6,0xf4292244);     ROUND4(d,a,b,c,x[7],10,0x432aff97);    ROUND4(c,d,a,b,x[14],15,0xab9423a7);   ROUND4(b,c,d,a,x[5],21,0xfc93a039);
    ROUND4(a,b,c,d,x[12],6,0x655b59c3);    ROUND4(d,a,b,c,x[3],10,0x8f0ccc92);    ROUND4(c,d,a,b,x[10],15,0xffeff47d);   ROUND4(b,c,d,a,x[1],21,0x85845dd1);
    ROUND4(a,b,c,d,x[8],6,0x6fa87e4f);     ROUND4(d,a,b,c,x[15],10,0xfe2ce6e0);   ROUND4(c,d,a,b,x[6],15,0xa3014314);    ROUND4(b,c,d,a,x[13],21,0x4e0811a1);
    ROUND4(a,b,c,d,x[4],6,0xf7537e82);     ROUND4(d,a,b,c,x[11],10,0xbd3af235);   ROUND4(c,d,a,b,x[2],15,0x2ad7d2bb);    ROUND4(b,c,d,a,x[9],21,0xeb86d391);
    
    *a2 = a + tmpA;
    *b2 = b + tmpB;
    *c2 = c + tmpC;
    *d2 = d + tmpD;
}