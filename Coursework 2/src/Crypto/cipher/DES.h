#ifndef DES_H
#define DES_H

#include "BlockCipher.h"

class TDES;

class DES : public BlockCipher
{
    friend class TDES;
    
    public :
    
        DES(uint8_t* key);
        DES(uint8_t *key, uint8_t *iv);
                
    private :
    
        virtual void encryptBlock(uint8_t *out, uint8_t *in);        
        virtual void decryptBlock(uint8_t *out, uint8_t *in);    
        void generateSubKeys(uint8_t *key);
    
        uint8_t subKeys[16][7];
};

#endif
