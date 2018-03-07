#ifndef SHA2_32_H
#define SHA2_32_H

#include <stdint.h>

enum SHA_32_TYPE
{
    SHA_224,
    SHA_256
};

class SHA2_32
{
    public :
    
        SHA2_32(SHA_32_TYPE type);
        void update(uint8_t *data, uint32_t length);
        void finalize(uint8_t *digest);
        
        static void computeHash(SHA_32_TYPE type, uint8_t *digest, uint8_t *data, uint32_t length);
        
    private : 
    
        static void computeBlock(uint32_t *h02, 
                                 uint32_t *h12, 
                                 uint32_t *h22, 
                                 uint32_t *h32, 
                                 uint32_t *h42, 
                                 uint32_t *h52, 
                                 uint32_t *h62,
                                 uint32_t *h72,
                                 uint8_t *buffer);

        SHA_32_TYPE type;
        uint32_t h0, h1, h2, h3, h4, h5, h6, h7;
        uint32_t totalBufferLength;
        uint8_t buffer[64];
        uint8_t bufferLength;   
};

#endif
