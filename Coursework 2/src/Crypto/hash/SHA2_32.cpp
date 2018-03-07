#include "SHA2_32.h"
#include <string.h>



static const uint8_t MASK = 0x0F;

#define W(t) (w[(t)] = SSIG1(w[((t)+14)&MASK]) + w[((t)+9)&MASK] + SSIG0(w[((t)+1)&MASK]) + w[t])

#define ROTL(W,N) (((W) << (N)) | ((W) >> (32-(N))))
#define ROTR(W,N) (rotRWord(W,N)) 
#define CH(X,Y,Z) (((X) & (Y)) ^ ((~(X)) & (Z)))
#define MAJ(X,Y,Z) (((X) & (Y)) ^ ((X) & (Z)) ^ ((Y) & (Z)))
#define BSIG0(X) (ROTR(X,2) ^ ROTR(X,13) ^ ROTR(X,22))
#define BSIG1(X) (ROTR(X,6) ^ ROTR(X,11) ^ ROTR(X,25))
#define SSIG0(X) (ROTR((X),7) ^ ROTR((X),18) ^ ((X) >> 3))
#define SSIG1(X) (ROTR((X),17) ^ ROTR((X),19) ^ ((X) >> 10))
#define R(A,B,C,D,E,F,G,H,T,K)  T1 = H + BSIG1(E) + CH(E,F,G) + K + (w[T] = revWord(buffer2[T])); \
                              T2 = BSIG0(A) + MAJ(A,B,C); \
                              D += T1; \
                              H = T1 + T2;
#define R2(A,B,C,D,E,F,G,H,T,K)  T1 = H + BSIG1(E) + CH(E,F,G) + K + W(T&MASK); \
                              T2 = BSIG0(A) + MAJ(A,B,C); \
                              D += T1; \
                              H = T1 + T2;
        
static const uint32_t H[] =
{
    // SHA-224
    0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939,
    0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4,
    
    // SHA-256      
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

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

static uint32_t rotRWord(const uint32_t w, const uint32_t n)
{
#ifdef __CC_ARM
    return __ror(w, n);
#else
    return (w >> n) | (w << (32-n));
#endif
}

SHA2_32::SHA2_32(SHA_32_TYPE t):
type(t),
totalBufferLength(0),
bufferLength(0)
{
    switch(type)
    {
        case SHA_224:
            h0 = H[0];
            h1 = H[1];
            h2 = H[2];
            h3 = H[3];
            h4 = H[4];
            h5 = H[5];
            h6 = H[6];
            h7 = H[7];
        break;
        
        case SHA_256:
            h0 = H[8];
            h1 = H[9];
            h2 = H[10];
            h3 = H[11];
            h4 = H[12];
            h5 = H[13];
            h6 = H[14];
            h7 = H[15];     
        break;
    }
}

void SHA2_32::update(uint8_t *data, uint32_t length)
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
    computeBlock(&h0,&h1,&h2,&h3,&h4,&h5,&h6,&h7,buffer);
    while(length-offset > 64)
    {
        memcpy(buffer, &data[offset], 64);
        computeBlock(&h0,&h1,&h2,&h3,&h4,&h5,&h6,&h7,buffer);
        offset += 64;
    }
    if(offset > (int)length)
        offset -= 64;
    bufferLength = length - offset;
    memcpy(buffer, &data[offset], bufferLength);
    totalBufferLength += length;
}

void SHA2_32::finalize(uint8_t *hash)
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
        computeBlock(&h0, &h1, &h2, &h3, &h4, &h5, &h6, &h7, buffer);
        memset(buffer, 0, 56);
    }
    
    uint64_t lengthBit = totalBufferLength << 3;
    uint32_t lengthBitLow = lengthBit;
    uint32_t lengthBitHigh = lengthBit >> 32;
    lengthBitLow = revWord(lengthBitLow);
    lengthBitHigh = revWord(lengthBitHigh);
    memcpy(&buffer[60], &lengthBitLow, 4);    
    memcpy(&buffer[56], &lengthBitHigh, 4);    
    computeBlock(&h0, &h1, &h2, &h3, &h4, &h5, &h6, &h7, buffer);

    hash2[0] = revWord(h0);
    hash2[1] = revWord(h1);
    hash2[2] = revWord(h2);
    hash2[3] = revWord(h3);
    hash2[4] = revWord(h4);
    hash2[5] = revWord(h5);
    hash2[6] = revWord(h6);

    
    if(type == SHA_256)
        hash2[7] = revWord(h7);
    
    // reset state
    switch(type)
    {
        case SHA_224:
            h0 = H[0];
            h1 = H[1];
            h2 = H[2];
            h3 = H[3];
            h4 = H[4];
            h5 = H[5];
            h6 = H[6];
            h7 = H[7];
        break;
        
        case SHA_256:
            h0 = H[8];
            h1 = H[9];
            h2 = H[10];
            h3 = H[11];
            h4 = H[12];
            h5 = H[13];
            h6 = H[14];
            h7 = H[15];     
        break;
    }
    totalBufferLength = 0;
    bufferLength = 0;
}

void SHA2_32::computeHash(SHA_32_TYPE type, uint8_t *hash, uint8_t *data, uint32_t length)
{
    uint32_t *hash2 = (uint32_t*)hash;
    
    uint32_t h[8];
    h[0] = H[type*8];
    h[1] = H[type*8+1];
    h[2] = H[type*8+2];
    h[3] = H[type*8+3];
    h[4] = H[type*8+4];
    h[5] = H[type*8+5];
    h[6] = H[type*8+6];
    h[7] = H[type*8+7];
    
    uint64_t lengthBit = length << 3;
    uint32_t padding;
    if(length % 64 < 56)
        padding = 56 - (length % 64);
    else
        padding = 56 + (64 - (length % 64));
        
    while(length >= 64)
    {
        computeBlock(h, &h[1], &h[2], &h[3], &h[4], &h[5], &h[6], &h[7], data);
        length -= 64;
        data += 64;
    }
    uint8_t buffer[64];
    memcpy(buffer, data,length); 
    buffer[length++] = 0x80;
    padding--;
    if(padding+length == 56)
        memset(&buffer[length], 0, padding);
    else
    {
        memset(&buffer[length], 0, 64-length);
        computeBlock(h, &h[1], &h[2], &h[3], &h[4], &h[5], &h[6], &h[7], buffer);
        memset(buffer, 0, 56);
    }
    
    uint32_t lengthBitLow = lengthBit;
    uint32_t lengthBitHigh = lengthBit >> 32;
    lengthBitLow = revWord(lengthBitLow);
    memcpy(&buffer[60], &lengthBitLow, 4);
    lengthBitHigh = revWord(lengthBitHigh);
    memcpy(&buffer[56], &lengthBitHigh, 4);    
    computeBlock(h, &h[1], &h[2], &h[3], &h[4], &h[5], &h[6], &h[7], buffer);

    hash2[0] = revWord(h[0]);
    hash2[1] = revWord(h[1]);
    hash2[2] = revWord(h[2]);
    hash2[3] = revWord(h[3]);
    hash2[4] = revWord(h[4]);
    hash2[5] = revWord(h[5]);
    hash2[6] = revWord(h[6]);

    
    if(type == SHA_256)
        hash2[7] = revWord(h[7]);
}


#ifdef __CC_ARM
__forceinline 
#endif 
void SHA2_32::computeBlock(uint32_t *h02, 
                        uint32_t *h12, 
                        uint32_t *h22, 
                        uint32_t *h32, 
                        uint32_t *h42, 
                        uint32_t *h52, 
                        uint32_t *h62,
                        uint32_t *h72,
                        uint8_t *buffer)
{
    uint32_t w[16];
    uint32_t *buffer2 = (uint32_t*)buffer;
    uint32_t a = *h02, b = *h12, c = *h22, d = *h32, e = *h42, f = *h52, g = *h62, h = *h72;
    uint32_t T1, T2;

    R(a,b,c,d,e,f,g,h,0,0x428a2f98)
    R(h,a,b,c,d,e,f,g,1,0x71374491)
    R(g,h,a,b,c,d,e,f,2,0xb5c0fbcf)
    R(f,g,h,a,b,c,d,e,3,0xe9b5dba5)
    R(e,f,g,h,a,b,c,d,4,0x3956c25b)
    R(d,e,f,g,h,a,b,c,5,0x59f111f1)
    R(c,d,e,f,g,h,a,b,6,0x923f82a4)
    R(b,c,d,e,f,g,h,a,7,0xab1c5ed5)

    R(a,b,c,d,e,f,g,h,8,0xd807aa98)
    R(h,a,b,c,d,e,f,g,9,0x12835b01)
    R(g,h,a,b,c,d,e,f,10,0x243185be)
    R(f,g,h,a,b,c,d,e,11,0x550c7dc3)
    R(e,f,g,h,a,b,c,d,12,0x72be5d74)
    R(d,e,f,g,h,a,b,c,13,0x80deb1fe)
    R(c,d,e,f,g,h,a,b,14,0x9bdc06a7)
    R(b,c,d,e,f,g,h,a,15,0xc19bf174)

    R2(a,b,c,d,e,f,g,h,16,0xe49b69c1)
    R2(h,a,b,c,d,e,f,g,17,0xefbe4786)
    R2(g,h,a,b,c,d,e,f,18,0x0fc19dc6)
    R2(f,g,h,a,b,c,d,e,19,0x240ca1cc)
    R2(e,f,g,h,a,b,c,d,20,0x2de92c6f)
    R2(d,e,f,g,h,a,b,c,21,0x4a7484aa)
    R2(c,d,e,f,g,h,a,b,22,0x5cb0a9dc)
    R2(b,c,d,e,f,g,h,a,23,0x76f988da)
    
    R2(a,b,c,d,e,f,g,h,24,0x983e5152)
    R2(h,a,b,c,d,e,f,g,25,0xa831c66d)
    R2(g,h,a,b,c,d,e,f,26,0xb00327c8)
    R2(f,g,h,a,b,c,d,e,27,0xbf597fc7)
    R2(e,f,g,h,a,b,c,d,28,0xc6e00bf3)
    R2(d,e,f,g,h,a,b,c,29,0xd5a79147)
    R2(c,d,e,f,g,h,a,b,30,0x06ca6351)
    R2(b,c,d,e,f,g,h,a,31,0x14292967) 

    R2(a,b,c,d,e,f,g,h,32,0x27b70a85)
    R2(h,a,b,c,d,e,f,g,33,0x2e1b2138)
    R2(g,h,a,b,c,d,e,f,34,0x4d2c6dfc)
    R2(f,g,h,a,b,c,d,e,35,0x53380d13)
    R2(e,f,g,h,a,b,c,d,36,0x650a7354)
    R2(d,e,f,g,h,a,b,c,37,0x766a0abb)
    R2(c,d,e,f,g,h,a,b,38,0x81c2c92e)
    R2(b,c,d,e,f,g,h,a,39,0x92722c85)
    
    R2(a,b,c,d,e,f,g,h,40,0xa2bfe8a1)
    R2(h,a,b,c,d,e,f,g,41,0xa81a664b)
    R2(g,h,a,b,c,d,e,f,42,0xc24b8b70)
    R2(f,g,h,a,b,c,d,e,43,0xc76c51a3)
    R2(e,f,g,h,a,b,c,d,44,0xd192e819)
    R2(d,e,f,g,h,a,b,c,45,0xd6990624)
    R2(c,d,e,f,g,h,a,b,46,0xf40e3585)
    R2(b,c,d,e,f,g,h,a,47,0x106aa070)
    
    R2(a,b,c,d,e,f,g,h,48,0x19a4c116)
    R2(h,a,b,c,d,e,f,g,49,0x1e376c08)
    R2(g,h,a,b,c,d,e,f,50,0x2748774c)
    R2(f,g,h,a,b,c,d,e,51,0x34b0bcb5)
    R2(e,f,g,h,a,b,c,d,52,0x391c0cb3)
    R2(d,e,f,g,h,a,b,c,53,0x4ed8aa4a)
    R2(c,d,e,f,g,h,a,b,54,0x5b9cca4f)
    R2(b,c,d,e,f,g,h,a,55,0x682e6ff3)
    
    R2(a,b,c,d,e,f,g,h,56,0x748f82ee)
    R2(h,a,b,c,d,e,f,g,57,0x78a5636f)
    R2(g,h,a,b,c,d,e,f,58,0x84c87814)
    R2(f,g,h,a,b,c,d,e,59,0x8cc70208)
    R2(e,f,g,h,a,b,c,d,60,0x90befffa)
    R2(d,e,f,g,h,a,b,c,61,0xa4506ceb)
    R2(c,d,e,f,g,h,a,b,62,0xbef9a3f7)
    R2(b,c,d,e,f,g,h,a,63,0xc67178f2)
    
    
    *h02 += a;
    *h12 += b;
    *h22 += c;
    *h32 += d;
    *h42 += e;
    *h52 += f;
    *h62 += g;
    *h72 += h;
}
