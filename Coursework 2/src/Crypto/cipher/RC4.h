#ifndef RC4_H
#define RC4_H

#include "StreamCipher.h"

class RC4 : public StreamCipher
{
    public :
    
        RC4(uint8_t *key, uint8_t keyLength);
        
    private :
    
        virtual uint8_t encryptByte(uint8_t in);
        virtual uint8_t decryptByte(uint8_t in);
                
        uint8_t s[256];
        uint8_t i,j;

};

#endif
