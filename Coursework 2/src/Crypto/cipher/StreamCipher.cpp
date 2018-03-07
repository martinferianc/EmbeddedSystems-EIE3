#include "StreamCipher.h"


StreamCipher::StreamCipher()
{
}

CIPHER_TYPE StreamCipher::getType() const
{
    return STREAM_CIPHER;
}

void StreamCipher::encrypt(uint8_t *out, uint8_t *in, uint32_t length)
{
    for(uint32_t i = 0; i < length; ++i)
        out[i] = encryptByte(in[i]);
}
        
void StreamCipher::decrypt(uint8_t *out, uint8_t *in, uint32_t length) 
{
    for(uint32_t i = 0; i < length; ++i)
        out[i] = decryptByte(in[i]);
}