#ifndef SHA2_224_H
#define SHA2_224_H

#include "HashAlgorithm.h"
#include "SHA2_32.h"

class SHA224 : public HashAlgorithm
{
    public :

        SHA224();

        virtual uint8_t outputSize() const;
        virtual void update(uint8_t *data, uint32_t length);
        virtual void finalize(uint8_t *hash);

        static void computeHash(uint8_t *hash, uint8_t *data, uint32_t length);

    private :
    
        SHA2_32 algo;
};

#endif
