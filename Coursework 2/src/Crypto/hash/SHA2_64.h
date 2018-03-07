#ifndef SHA2_64_H
#define SHA2_64_H

#include <stdint.h>

enum SHA2_64_TYPE
{
    SHA_384,
    SHA_512
};

class SHA2_64
{
    public :

        SHA2_64(SHA2_64_TYPE type);
        
        void update(uint8_t *data, uint32_t length);
        void finalize(uint8_t *hash);
        
        static void computeHash(SHA2_64_TYPE type, uint8_t *hash, uint8_t *data, uint32_t length);

    private :
    
            static void computeBlock(uint64_t *h02, 
                                 uint64_t *h12, 
                                 uint64_t *h22, 
                                 uint64_t *h32, 
                                 uint64_t *h42, 
                                 uint64_t *h52, 
                                 uint64_t *h62,
                                 uint64_t *h72,
                                 uint8_t *buffer);
                                 
        SHA2_64_TYPE type;
        uint64_t h0, h1, h2, h3, h4, h5, h6, h7;
        uint32_t totalBufferLength;
        uint8_t buffer[128];
        uint8_t bufferLength;        
};

#endif
