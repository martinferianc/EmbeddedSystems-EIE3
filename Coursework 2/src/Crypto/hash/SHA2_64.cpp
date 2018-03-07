#include "SHA2_64.h"
#include <string.h>


static const uint64_t H[] =
{
    // SHA-384
    0xcbbb9d5dc1059ed8, 0x629a292a367cd507, 0x9159015a3070dd17, 0x152fecd8f70e5939,
    0x67332667ffc00b31, 0x8eb44a8768581511, 0xdb0c2e0d64f98fa7, 0x47b5481dbefa4fa4,

    // SHA-512
    0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
    0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179
};

static uint64_t revWord(uint64_t w)
{
#ifdef __CC_ARM
    return __rev(w >> 32) 
         | ((uint64_t)(__rev(w)) << 32);
#else
    return (w >> 56)
         | ((w & 0x00FF000000000000) >> 40)
         | ((w & 0x0000FF0000000000) >> 24)
         | ((w & 0x000000FF00000000) >> 8)
         | ((w & 0x00000000FF000000) << 8)
         | ((w & 0x0000000000FF0000) << 24)
         | ((w & 0x000000000000FF00) << 40)
         | ((w & 0x00000000000000FF) << 56);
#endif
}

#define ROTL(W,N) (((W) << (N)) | ((W) >> (64-(N))))
#define ROTR(W,N) (((W) >> (N)) | ((W) << (64-(N))))
#define CH(X,Y,Z) (((X) & (Y)) ^ ((~(X)) & (Z)))
#define MAJ(X,Y,Z) (((X) & (Y)) ^ ((X) & (Z)) ^ ((Y) & (Z)))
#define BSIG0(X) (ROTR(X,28) ^ ROTR(X,34) ^ ROTR(X,39))
#define BSIG1(X) (ROTR(X,14) ^ ROTR(X,18) ^ ROTR(X,41))
#define SSIG0(X) (ROTR((X),1) ^ ROTR((X),8) ^ ((X) >> 7))
#define SSIG1(X) (ROTR((X),19) ^ ROTR((X),61) ^ ((X) >> 6))

#define R(A,B,C,D,E,F,G,H,K,T)  T1 = H + BSIG1(E) + CH(E,F,G) + K + w[T]; \
                              T2 = BSIG0(A) + MAJ(A,B,C); \
                              D += T1; \
                              H = T1 + T2;    
                          

SHA2_64::SHA2_64(SHA2_64_TYPE t):
type(t),
totalBufferLength(0),
bufferLength(0)
{
    switch(type)
    {
        case SHA_384:
            h0 = H[0];
            h1 = H[1];
            h2 = H[2];
            h3 = H[3];
            h4 = H[4];
            h5 = H[5];
            h6 = H[6];
            h7 = H[7];
        break;
        
        case SHA_512:
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

void SHA2_64::update(uint8_t *data, uint32_t length)
{
    if((int)length < 128-bufferLength)
    {
        memcpy(&buffer[bufferLength], data, length);
        bufferLength += length;
        totalBufferLength += length;
        return;
    }
    int offset = 128-bufferLength;
    memcpy(&buffer[bufferLength], data, offset);
    computeBlock(&h0,&h1,&h2,&h3,&h4,&h5,&h6,&h7,buffer);
    while(length-offset > 128)
    {
        memcpy(buffer, &data[offset], 128);
        computeBlock(&h0,&h1,&h2,&h3,&h4,&h5,&h6,&h7,buffer);
        offset += 128;
    }
    if(offset > (int)length)
        offset -= 128;
    bufferLength = length - offset;
    memcpy(buffer, &data[offset], bufferLength);
    totalBufferLength += length;
}

void SHA2_64::finalize(uint8_t *hash)
{
    uint64_t *hash2 = (uint64_t*)hash;
    uint64_t lengthBit = totalBufferLength << 3;
    uint32_t padding;
    if(totalBufferLength % 128 < 112)
        padding = 112 - (totalBufferLength % 128);
    else
        padding = 112 + (128 - (totalBufferLength % 128));

    buffer[bufferLength++] = 0x80;
    padding--;
    if(padding+bufferLength == 112)
        memset(&buffer[bufferLength], 0, padding);
    else
    {
        memset(&buffer[bufferLength], 0, 64-bufferLength);
        computeBlock(&h0, &h1, &h2, &h3, &h4, &h5, &h6, &h7, buffer);
        memset(buffer, 0, 112);
    }
    
    lengthBit = revWord(lengthBit);
    memcpy(&buffer[120], &lengthBit, 8);    
    memset(&buffer[112], 0, 8);    
    computeBlock(&h0, &h1, &h2, &h3, &h4, &h5, &h6, &h7, buffer);


    hash2[0] = revWord(h0);
    hash2[1] = revWord(h1);
    hash2[2] = revWord(h2);
    hash2[3] = revWord(h3);
    hash2[4] = revWord(h4);
    hash2[5] = revWord(h5);


    if(type == SHA_512)
    {
        hash2[6] = revWord(h6);
        hash2[7] = revWord(h7);
    }
    
    // reset state
    switch(type)
    {
        case SHA_384:
            h0 = H[0];
            h1 = H[1];
            h2 = H[2];
            h3 = H[3];
            h4 = H[4];
            h5 = H[5];
            h6 = H[6];
            h7 = H[7];
        break;
        
        case SHA_512:
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

void SHA2_64::computeHash(SHA2_64_TYPE type, uint8_t *hash, uint8_t *data, uint32_t length)
{
    uint64_t *hash2 = (uint64_t*)hash;
    uint64_t lengthBit = length * 8;
    uint64_t h0 = H[type*8], h1 = H[type*8+1], h2 = H[type*8+2], h3 = H[type*8+3];
    uint64_t h4 = H[type*8+4], h5 = H[type*8+5], h6 = H[type*8+6], h7 = H[type*8+7];
    
    int padding;
    if(length % 128 < 112)
        padding = 112 - (length % 128);
    else
        padding = 112 + (128 - (length % 128));
        
    while(length >= 128)
    {
        computeBlock(&h0, &h1, &h2, &h3, &h4, &h5, &h6, &h7, data);
        data += 128;
        length -= 128;
    }
    uint8_t buffer[128];
    memcpy(buffer, data,length); 
    buffer[length] = 0x80;
    length++;
    padding--;

    if(padding+length == 112)
        memset(&buffer[length], 0, padding);
    else
    {
        memset(&buffer[length], 0, 128-length);
        computeBlock(&h0, &h1, &h2, &h3, &h4, &h5, &h6, &h7, buffer);
        memset(buffer, 0, 112);
    }
    
    lengthBit = revWord(lengthBit);
    memset(&buffer[112], 0, 8); 
    memcpy(&buffer[120], &lengthBit, 8);
    computeBlock(&h0, &h1, &h2, &h3, &h4, &h5, &h6, &h7, buffer);

    hash2[0] = revWord(h0);
    hash2[1] = revWord(h1);
    hash2[2] = revWord(h2);
    hash2[3] = revWord(h3);
    hash2[4] = revWord(h4);
    hash2[5] = revWord(h5);


    if(type == SHA_512)
    {
        hash2[6] = revWord(h6);
        hash2[7] = revWord(h7);
    }
}

void SHA2_64::computeBlock(uint64_t *h02, 
                     uint64_t *h12, 
                     uint64_t *h22, 
                     uint64_t *h32, 
                     uint64_t *h42, 
                     uint64_t *h52, 
                     uint64_t *h62,
                     uint64_t *h72,
                     uint8_t *buffer)
{
    uint64_t w[80];
    uint64_t *buffer2 = (uint64_t*)buffer;

    w[0] = revWord(buffer2[0]);
    w[1] = revWord(buffer2[1]);
    w[2] = revWord(buffer2[2]);
    w[3] = revWord(buffer2[3]);
    w[4] = revWord(buffer2[4]);
    w[5] = revWord(buffer2[5]);
    w[6] = revWord(buffer2[6]);
    w[7] = revWord(buffer2[7]); 
    w[8] = revWord(buffer2[8]);
    w[9] = revWord(buffer2[9]);
    w[10] = revWord(buffer2[10]);
    w[11] = revWord(buffer2[11]);
    w[12] = revWord(buffer2[12]);
    w[13] = revWord(buffer2[13]);
    w[14] = revWord(buffer2[14]);
    w[15] = revWord(buffer2[15]);     
    
    for(int t = 16; t < 80; ++t)
        w[t] = SSIG1(w[t-2]) + w[t-7] + SSIG0(w[t-15]) + w[t-16];
    
    uint64_t a = *h02, b = *h12, c = *h22, d = *h32, e = *h42, f = *h52, g = *h62, h = *h72;
    uint64_t T1, T2;
    

    R(a,b,c,d,e,f,g,h,0x428a2f98d728ae22,0)
    R(h,a,b,c,d,e,f,g,0x7137449123ef65cd,1)
    R(g,h,a,b,c,d,e,f,0xb5c0fbcfec4d3b2f,2)
    R(f,g,h,a,b,c,d,e,0xe9b5dba58189dbbc,3)
    R(e,f,g,h,a,b,c,d,0x3956c25bf348b538,4)
    R(d,e,f,g,h,a,b,c,0x59f111f1b605d019,5)
    R(c,d,e,f,g,h,a,b,0x923f82a4af194f9b,6)
    R(b,c,d,e,f,g,h,a,0xab1c5ed5da6d8118,7)

    R(a,b,c,d,e,f,g,h,0xd807aa98a3030242,8)
    R(h,a,b,c,d,e,f,g,0x12835b0145706fbe,9)
    R(g,h,a,b,c,d,e,f,0x243185be4ee4b28c,10)
    R(f,g,h,a,b,c,d,e,0x550c7dc3d5ffb4e2,11)
    R(e,f,g,h,a,b,c,d,0x72be5d74f27b896f,12)
    R(d,e,f,g,h,a,b,c,0x80deb1fe3b1696b1,13)
    R(c,d,e,f,g,h,a,b,0x9bdc06a725c71235,14)
    R(b,c,d,e,f,g,h,a,0xc19bf174cf692694,15)
    
    
    R(a,b,c,d,e,f,g,h,0xe49b69c19ef14ad2,16)
    R(h,a,b,c,d,e,f,g,0xefbe4786384f25e3,17)
    R(g,h,a,b,c,d,e,f,0x0fc19dc68b8cd5b5,18)
    R(f,g,h,a,b,c,d,e,0x240ca1cc77ac9c65,19)
    R(e,f,g,h,a,b,c,d,0x2de92c6f592b0275,20)
    R(d,e,f,g,h,a,b,c,0x4a7484aa6ea6e483,21)
    R(c,d,e,f,g,h,a,b,0x5cb0a9dcbd41fbd4,22)
    R(b,c,d,e,f,g,h,a,0x76f988da831153b5,23)
    
    R(a,b,c,d,e,f,g,h,0x983e5152ee66dfab,24)
    R(h,a,b,c,d,e,f,g,0xa831c66d2db43210,25)
    R(g,h,a,b,c,d,e,f,0xb00327c898fb213f,26)
    R(f,g,h,a,b,c,d,e,0xbf597fc7beef0ee4,27)
    R(e,f,g,h,a,b,c,d,0xc6e00bf33da88fc2,28)
    R(d,e,f,g,h,a,b,c,0xd5a79147930aa725,29)
    R(c,d,e,f,g,h,a,b,0x06ca6351e003826f,30)
    R(b,c,d,e,f,g,h,a,0x142929670a0e6e70,31) 
    
    
    R(a,b,c,d,e,f,g,h,0x27b70a8546d22ffc,32)
    R(h,a,b,c,d,e,f,g,0x2e1b21385c26c926,33)
    R(g,h,a,b,c,d,e,f,0x4d2c6dfc5ac42aed,34)
    R(f,g,h,a,b,c,d,e,0x53380d139d95b3df,35)
    R(e,f,g,h,a,b,c,d,0x650a73548baf63de,36)
    R(d,e,f,g,h,a,b,c,0x766a0abb3c77b2a8,37)
    R(c,d,e,f,g,h,a,b,0x81c2c92e47edaee6,38)
    R(b,c,d,e,f,g,h,a,0x92722c851482353b,39)
    
    R(a,b,c,d,e,f,g,h,0xa2bfe8a14cf10364,40)
    R(h,a,b,c,d,e,f,g,0xa81a664bbc423001,41)
    R(g,h,a,b,c,d,e,f,0xc24b8b70d0f89791,42)
    R(f,g,h,a,b,c,d,e,0xc76c51a30654be30,43)
    R(e,f,g,h,a,b,c,d,0xd192e819d6ef5218,44)
    R(d,e,f,g,h,a,b,c,0xd69906245565a910,45)
    R(c,d,e,f,g,h,a,b,0xf40e35855771202a,46)
    R(b,c,d,e,f,g,h,a,0x106aa07032bbd1b8,47)

    R(a,b,c,d,e,f,g,h,0x19a4c116b8d2d0c8,48)
    R(h,a,b,c,d,e,f,g,0x1e376c085141ab53,49)
    R(g,h,a,b,c,d,e,f,0x2748774cdf8eeb99,50)
    R(f,g,h,a,b,c,d,e,0x34b0bcb5e19b48a8,51)
    R(e,f,g,h,a,b,c,d,0x391c0cb3c5c95a63,52)
    R(d,e,f,g,h,a,b,c,0x4ed8aa4ae3418acb,53)
    R(c,d,e,f,g,h,a,b,0x5b9cca4f7763e373,54)
    R(b,c,d,e,f,g,h,a,0x682e6ff3d6b2b8a3,55)
    
    R(a,b,c,d,e,f,g,h,0x748f82ee5defb2fc,56)
    R(h,a,b,c,d,e,f,g,0x78a5636f43172f60,57)
    R(g,h,a,b,c,d,e,f,0x84c87814a1f0ab72,58)
    R(f,g,h,a,b,c,d,e,0x8cc702081a6439ec,59)
    R(e,f,g,h,a,b,c,d,0x90befffa23631e28,60)
    R(d,e,f,g,h,a,b,c,0xa4506cebde82bde9,61)
    R(c,d,e,f,g,h,a,b,0xbef9a3f7b2c67915,62)
    R(b,c,d,e,f,g,h,a,0xc67178f2e372532b,63)

    R(a,b,c,d,e,f,g,h,0xca273eceea26619c,64)
    R(h,a,b,c,d,e,f,g,0xd186b8c721c0c207,65)
    R(g,h,a,b,c,d,e,f,0xeada7dd6cde0eb1e,66)
    R(f,g,h,a,b,c,d,e,0xf57d4f7fee6ed178,67)
    R(e,f,g,h,a,b,c,d,0x06f067aa72176fba,68)
    R(d,e,f,g,h,a,b,c,0x0a637dc5a2c898a6,69)
    R(c,d,e,f,g,h,a,b,0x113f9804bef90dae,70)
    R(b,c,d,e,f,g,h,a,0x1b710b35131c471b,71)

    R(a,b,c,d,e,f,g,h,0x28db77f523047d84,72)
    R(h,a,b,c,d,e,f,g,0x32caab7b40c72493,73)
    R(g,h,a,b,c,d,e,f,0x3c9ebe0a15c9bebc,74)
    R(f,g,h,a,b,c,d,e,0x431d67c49c100d4c,75)
    R(e,f,g,h,a,b,c,d,0x4cc5d4becb3e42b6,76)
    R(d,e,f,g,h,a,b,c,0x597f299cfc657e2a,77)
    R(c,d,e,f,g,h,a,b,0x5fcb6fab3ad6faec,78)
    R(b,c,d,e,f,g,h,a,0x6c44198c4a475817,79)
            
    *h02 += a;
    *h12 += b;
    *h22 += c;
    *h32 += d;
    *h42 += e;
    *h52 += f;
    *h62 += g;
    *h72 += h;
}