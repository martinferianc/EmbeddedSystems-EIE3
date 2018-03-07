#ifndef SHA1_H
#define SHA1_H

#include "HashAlgorithm.h"


class SHA1 : public HashAlgorithm
{
    public :
    
        SHA1();

        virtual uint8_t outputSize() const;        
        virtual void update(uint8_t *data, uint32_t length);
        virtual void finalize(uint8_t *hash);
        
        static void computeHash(uint8_t *hash, uint8_t *data, uint32_t length);
        
    private :
        static void computeBlock(uint32_t *h02, uint32_t *h12, uint32_t *h22, uint32_t *h32, uint32_t *h42, uint8_t *buffer);
    
        uint32_t h0, h1, h2, h3, h4;
        uint32_t totalBufferLength;
        uint8_t buffer[64];
        uint8_t bufferLength;       
};

#endif
