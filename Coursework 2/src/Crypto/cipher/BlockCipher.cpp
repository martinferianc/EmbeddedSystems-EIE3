#include "BlockCipher.h"
#include <string.h>

BlockCipher::BlockCipher(uint32_t bs, BLOCK_CIPHER_MODE m, uint8_t *iv):
Cipher(),
blockSize(bs),
mode(m),
IV(0)
{
    if(mode == CBC_MODE)
    {
        IV = new uint8_t[blockSize];
        memcpy(IV, iv, blockSize); 
    }
}

BlockCipher::~BlockCipher()
{
    if(IV != 0)
        delete[] IV;
}

CIPHER_TYPE BlockCipher::getType() const
{
    return BLOCK_CIPHER;
}

uint32_t BlockCipher::getBlockSize() const
{
    return blockSize;
}

void BlockCipher::encrypt(uint8_t *out, uint8_t *in, uint32_t length)
{
    uint8_t *tmp = 0;
    if(mode == CBC_MODE)
        tmp = new uint8_t[getBlockSize()];
    for(uint32_t i = 0; i < length; i += getBlockSize())
    {
        if(mode == CBC_MODE)
        {
            memcpy(tmp, &in[i], getBlockSize());
            for(int j = 0; j < (int)getBlockSize(); ++j)
                tmp[j] ^= IV[j];
                
            encryptBlock(&out[i], tmp);
        
            memcpy(IV, &out[i], getBlockSize());
        }
        else        
            encryptBlock(&out[i], &in[i]);
    }
    if(mode == CBC_MODE)
        delete[] tmp;
}

void BlockCipher::decrypt(uint8_t *out, uint8_t *in, uint32_t length)
{
    for(uint32_t i = 0; i < length; i += getBlockSize())
    {
        if(mode == CBC_MODE)
        {
            decryptBlock(&out[i], &in[i]);
            for(int j = 0; j < (int)getBlockSize(); ++j)
                out[i+j] ^= IV[j];
        
            memcpy(IV, &in[i], getBlockSize());
        }
        else        
            decryptBlock(&out[i], &in[i]);
    }
}
