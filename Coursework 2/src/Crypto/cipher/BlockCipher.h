#ifndef BLOCK_CIPHER_H
#define BLOCK_CIPHER_H

#include "Cipher.h"

enum BLOCK_CIPHER_MODE
{
    ECB_MODE,
    CBC_MODE
};

class BlockCipher : public Cipher
{
    public :
    
        BlockCipher(uint32_t bs, BLOCK_CIPHER_MODE m, uint8_t *iv = 0);
        virtual ~BlockCipher();
        
        virtual CIPHER_TYPE getType() const;        
        uint32_t getBlockSize() const;

        virtual void encrypt(uint8_t *out, uint8_t *in, uint32_t length);        
        virtual void decrypt(uint8_t *out, uint8_t *in, uint32_t length);        

    protected :
    
        virtual void encryptBlock(uint8_t *out, uint8_t *in) = 0;
        virtual void decryptBlock(uint8_t *out, uint8_t *in) = 0;

    private :
    
        uint32_t blockSize;
        BLOCK_CIPHER_MODE mode;
        uint8_t *IV;
};

#endif
