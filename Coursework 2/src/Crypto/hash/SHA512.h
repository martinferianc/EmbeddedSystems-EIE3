#ifndef SHA2_512_H
#define SHA2_512_H

#include "HashAlgorithm.h"
#include "SHA2_64.h"


class SHA512 : public HashAlgorithm
{
    public :

        SHA512();
        
        virtual uint8_t outputSize() const;
        virtual void update(uint8_t *data, uint32_t length);
        virtual void finalize(uint8_t *hash);

        static void computeHash(uint8_t *hash, uint8_t *data, uint32_t length);

    private :
    
        SHA2_64 algo;
};

#endif
