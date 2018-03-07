#ifndef HASH_ALGORITHM_H
#define HASH_ALGORITHM_H

#include <stdint.h>

class HashAlgorithm
{
    public :
    
        virtual ~HashAlgorithm();
        
        virtual uint8_t outputSize() const = 0;
        virtual void update(uint8_t *data, uint32_t length) = 0;
        virtual void finalize(uint8_t *hash) = 0;
};

#endif
