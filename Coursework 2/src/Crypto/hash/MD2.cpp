/**
    Implementation of MD2 as described here:
    http://tools.ietf.org/html/rfc1319
*/

#include "MD2.h"
#include <string.h>


static const uint8_t s[] =
{
    0x29, 0x2E, 0x43, 0xC9, 0xA2, 0xD8, 0x7C, 0x01, 0x3D, 0x36, 0x54, 0xA1, 0xEC, 0xF0, 0x06, 0x13, 
    0x62, 0xA7, 0x05, 0xF3, 0xC0, 0xC7, 0x73, 0x8C, 0x98, 0x93, 0x2B, 0xD9, 0xBC, 0x4C, 0x82, 0xCA, 
    0x1E, 0x9B, 0x57, 0x3C, 0xFD, 0xD4, 0xE0, 0x16, 0x67, 0x42, 0x6F, 0x18, 0x8A, 0x17, 0xE5, 0x12, 
    0xBE, 0x4E, 0xC4, 0xD6, 0xDA, 0x9E, 0xDE, 0x49, 0xA0, 0xFB, 0xF5, 0x8E, 0xBB, 0x2F, 0xEE, 0x7A, 
    0xA9, 0x68, 0x79, 0x91, 0x15, 0xB2, 0x07, 0x3F, 0x94, 0xC2, 0x10, 0x89, 0x0B, 0x22, 0x5F, 0x21,
    0x80, 0x7F, 0x5D, 0x9A, 0x5A, 0x90, 0x32, 0x27, 0x35, 0x3E, 0xCC, 0xE7, 0xBF, 0xF7, 0x97, 0x03, 
    0xFF, 0x19, 0x30, 0xB3, 0x48, 0xA5, 0xB5, 0xD1, 0xD7, 0x5E, 0x92, 0x2A, 0xAC, 0x56, 0xAA, 0xC6, 
    0x4F, 0xB8, 0x38, 0xD2, 0x96, 0xA4, 0x7D, 0xB6, 0x76, 0xFC, 0x6B, 0xE2, 0x9C, 0x74, 0x04, 0xF1, 
    0x45, 0x9D, 0x70, 0x59, 0x64, 0x71, 0x87, 0x20, 0x86, 0x5B, 0xCF, 0x65, 0xE6, 0x2D, 0xA8, 0x02, 
    0x1B, 0x60, 0x25, 0xAD, 0xAE, 0xB0, 0xB9, 0xF6, 0x1C, 0x46, 0x61, 0x69, 0x34, 0x40, 0x7E, 0x0F, 
    0x55, 0x47, 0xA3, 0x23, 0xDD, 0x51, 0xAF, 0x3A, 0xC3, 0x5C, 0xF9, 0xCE, 0xBA, 0xC5, 0xEA, 0x26, 
    0x2C, 0x53, 0x0D, 0x6E, 0x85, 0x28, 0x84, 0x09, 0xD3, 0xDF, 0xCD, 0xF4, 0x41, 0x81, 0x4D, 0x52, 
    0x6A, 0xDC, 0x37, 0xC8, 0x6C, 0xC1, 0xAB, 0xFA, 0x24, 0xE1, 0x7B, 0x08, 0x0C, 0xBD, 0xB1, 0x4A, 
    0x78, 0x88, 0x95, 0x8B, 0xE3, 0x63, 0xE8, 0x6D, 0xE9, 0xCB, 0xD5, 0xFE, 0x3B, 0x00, 0x1D, 0x39, 
    0xF2, 0xEF, 0xB7, 0x0E, 0x66, 0x58, 0xD0, 0xE4, 0xA6, 0x77, 0x72, 0xF8, 0xEB, 0x75, 0x4B, 0x0A, 
    0x31, 0x44, 0x50, 0xB4, 0x8F, 0xED, 0x1F, 0x1A, 0xDB, 0x99, 0x8D, 0x33, 0x9F, 0x11, 0x83, 0x14
};


MD2::MD2():
HashAlgorithm(),
bufferLength(0),
l(0)
{
    memset(checksum, 0, 16);
    memset(x, 0, 16);
}

uint8_t MD2::outputSize() const
{
    return 16;
}

void MD2::update(uint8_t *data, uint32_t length)
{ 
    if(bufferLength == 0)
    {
        while(length >= 16)
        {
            computeBlock(checksum, x, &l, data);
            length -= 16;
            data += 16;
        }
        bufferLength = length;
        memcpy(buffer, data, length);
    }
    else if((int)length < 16-bufferLength)
    {
        memcpy(&buffer[bufferLength], data, length);
        bufferLength += length;
    }
    else
    {
        int offset = 16-bufferLength;
        memcpy(&buffer[bufferLength], data, offset);
        computeBlock(checksum, x, &l, buffer);
        data += offset;
        length -= offset;
        while(length >= 16)
        {
            computeBlock(checksum, x, &l, data);
            data += 16;
            length -= 16;
        }
        bufferLength = length;
        memcpy(buffer, &data, length);
    }
    
}

void MD2::finalize(uint8_t *hash)
{
    // compute what's left data the buffer
    int padding = 16 - bufferLength;
    memset(&buffer[bufferLength], padding, padding);
    computeBlock(checksum, x, &l, buffer);
    computeBlock(checksum, x, &l, checksum);
    memcpy(hash, x, 16);

    uint32_t *x2 = (uint32_t*)x;
    uint32_t *checksum2 = (uint32_t*)checksum;

    // reset state
    bufferLength = 0;
    l = 0;
    checksum2[0] = x2[0] = 0;
    checksum2[1] = x2[1] = 0;
    checksum2[2] = x2[2] = 0;
    checksum2[3] = x2[3] = 0;
}

void MD2::computeHash(uint8_t *hash, uint8_t *data, uint32_t length)
{
    uint8_t x[48];
    uint8_t checksum[16];
    uint8_t buffer[16];
    memset(x, 0, 16);
    memset(checksum, 0, 16);
    uint8_t l = 0;
    while(length >= 16)
    {
        computeBlock(checksum, x, &l, data);
        length -= 16;
        data += 16;
    }

    memcpy(buffer, data, length);
    uint8_t padding = 16-length;
    memset(&buffer[length], padding, padding);
    computeBlock(checksum, x, &l, buffer);
    computeBlock(checksum,x, &l, checksum);
    memcpy(hash, x, 16);
}

#ifdef __CC_ARM
__forceinline 
#endif
void MD2::computeBlock(uint8_t *checksum2, uint8_t *x2, uint8_t *l2, uint8_t *buffer2)
{
    if(checksum2 != buffer2)
    {
        #ifdef __CC_ARM
            #pragma unroll_completely   
        #endif
        for(int j = 0; j < 16; ++j)
        {
            uint8_t c = buffer2[j];
            *l2 = (checksum2[j] ^= s[c^(*l2)]);
        }
    }
    
    uint32_t *x3 = (uint32_t*)x2;
    uint32_t *buffer3 = (uint32_t*)buffer2;
    
    x3[4] = buffer3[0];
    x3[5] = buffer3[1];
    x3[6] = buffer3[2];
    x3[7] = buffer3[3];
    for(int j = 0; j < 4; ++j)
        x3[8+j] = x3[4+j] ^ x3[j];
    
    uint8_t t = 0;
    #ifdef __CC_ARM
        #pragma unroll_completely   
    #endif
    for(int j = 0; j < 18; ++j)
    {
        t = (x2[0] ^= s[t]);
        t = (x2[1] ^= s[t]);
        t = (x2[2] ^= s[t]);
        t = (x2[3] ^= s[t]);
        t = (x2[4] ^= s[t]);
        t = (x2[5] ^= s[t]);
        t = (x2[6] ^= s[t]);
        t = (x2[7] ^= s[t]);
        t = (x2[8] ^= s[t]);
        t = (x2[9] ^= s[t]);
        t = (x2[10] ^= s[t]);
        t = (x2[11] ^= s[t]);
        t = (x2[12] ^= s[t]);
        t = (x2[13] ^= s[t]);
        t = (x2[14] ^= s[t]);
        t = (x2[15] ^= s[t]);
        t = (x2[16] ^= s[t]);
        t = (x2[17] ^= s[t]);
        t = (x2[18] ^= s[t]);
        t = (x2[19] ^= s[t]);
        t = (x2[20] ^= s[t]);
        t = (x2[21] ^= s[t]);
        t = (x2[22] ^= s[t]);
        t = (x2[23] ^= s[t]);            
        t = (x2[24] ^= s[t]);
        t = (x2[25] ^= s[t]);
        t = (x2[26] ^= s[t]);
        t = (x2[27] ^= s[t]);
        t = (x2[28] ^= s[t]);
        t = (x2[29] ^= s[t]);
        t = (x2[30] ^= s[t]);
        t = (x2[31] ^= s[t]);
        t = (x2[32] ^= s[t]);
        t = (x2[33] ^= s[t]);
        t = (x2[34] ^= s[t]);
        t = (x2[35] ^= s[t]);
        t = (x2[36] ^= s[t]);
        t = (x2[37] ^= s[t]);
        t = (x2[38] ^= s[t]);
        t = (x2[39] ^= s[t]);
        t = (x2[40] ^= s[t]);
        t = (x2[41] ^= s[t]);
        t = (x2[42] ^= s[t]);
        t = (x2[43] ^= s[t]);
        t = (x2[44] ^= s[t]);
        t = (x2[45] ^= s[t]);
        t = (x2[46] ^= s[t]);
        t = (x2[47] ^= s[t]);            
        
        t += j;
    }
}