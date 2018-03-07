#ifndef TDES_H
#define TDES_H

#include "DES.h"

class TDES : public BlockCipher
{
    public :
    
        TDES(uint8_t *key1, uint8_t *key2, uint8_t *key3);
        TDES(uint8_t *key1, uint8_t *key2, uint8_t *key3, uint8_t *iv);
            
    private :
        
        virtual void encryptBlock(uint8_t *out, uint8_t *in);        
        virtual void decryptBlock(uint8_t *out, uint8_t *in);    
    
    
        DES a,b,c;
};

#endif