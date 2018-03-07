#ifndef HMAC_H
#define HMAC_H

#include "HashAlgorithm.h"

class HMAC
{
    public :
        
        HMAC(HashAlgorithm *hashAlgo, uint8_t *k, uint32_t kl);
        virtual ~HMAC();
        
        void update(uint8_t *data, uint32_t length);
        void finalize(uint8_t *hash);
        
    private :
    
        HashAlgorithm *algo;
        uint8_t key[64];
        uint32_t keyLength;
};

#endif
