#ifndef MD4_H
#define MD4_H

#include "HashAlgorithm.h"

class MD4: public HashAlgorithm
{
    public :
    
        MD4();
        
        virtual uint8_t outputSize() const;
        virtual void update(uint8_t *data, uint32_t length);
        virtual void finalize(uint8_t *hash);
        
        static void computeHash(uint8_t *hash, uint8_t *data, uint32_t length);
        
    private :
    
        static void computeRounds(uint32_t *a2, uint32_t *b2, uint32_t *c2, uint32_t *d2, uint8_t *buffer);
        
        uint32_t a,b,c,d;
        uint32_t totalBufferLength;
        uint8_t buffer[64];
        uint8_t bufferLength;
};

#endif
