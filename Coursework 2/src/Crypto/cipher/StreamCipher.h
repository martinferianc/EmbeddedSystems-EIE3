#ifndef STREAM_CIPHER_H
#define STREAM_CIPHER_H

#include "Cipher.h"

class StreamCipher : public Cipher
{
    public :
    
        StreamCipher();
        
        virtual CIPHER_TYPE getType() const;        

        virtual void encrypt(uint8_t *out, uint8_t *in, uint32_t length);        
        virtual void decrypt(uint8_t *out, uint8_t *in, uint32_t length);        

    protected :
    
        virtual uint8_t encryptByte(uint8_t in) = 0;
        virtual uint8_t decryptByte(uint8_t in) = 0;

};

#endif
