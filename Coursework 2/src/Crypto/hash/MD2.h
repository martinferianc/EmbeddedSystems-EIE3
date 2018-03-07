#ifndef MD2_H
#define MD2_H

#include "HashAlgorithm.h"

class MD2 : public HashAlgorithm
{
    public :
    
        MD2();
        
        virtual uint8_t outputSize() const;
        virtual void update(uint8_t *data, uint32_t length);
        virtual void finalize(uint8_t *hash);
        
        static void computeHash(uint8_t *hash, uint8_t *data, uint32_t length);
        
    private :
          
        static void computeBlock(uint8_t *checksum, uint8_t *x, uint8_t *l2, uint8_t *buffer2);
    
        uint8_t bufferLength;
        uint8_t l;
        uint8_t buffer[16];
        uint8_t checksum[16];
        uint8_t x[48];
};


#endif
