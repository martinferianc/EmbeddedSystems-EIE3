#ifndef CIPHER_H
#define CIPHER_H

#include <stdint.h>

enum CIPHER_TYPE
{
    STREAM_CIPHER,
    BLOCK_CIPHER
};

class Cipher
{
    public :
    
        virtual ~Cipher();
        
        virtual void encrypt(uint8_t *out, uint8_t *in, uint32_t length) = 0;        
        virtual void decrypt(uint8_t *out, uint8_t *in, uint32_t length) = 0;        

        virtual CIPHER_TYPE getType() const = 0;

};


#endif
