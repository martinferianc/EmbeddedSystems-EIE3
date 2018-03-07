#include "HMAC.h"
#include <string.h>

HMAC::HMAC(HashAlgorithm *hashAlgo, uint8_t *k, uint32_t kl):
algo(hashAlgo),
keyLength(kl)
{
    memcpy(key, k, keyLength);
    uint8_t buffer[64];
    memcpy(buffer, key, keyLength);
    memset(&buffer[keyLength], 0, 64-keyLength);  
    
    for(int i = 0; i < 64; ++i)
        buffer[i] ^= 0x36;
        
    algo->update(buffer, 64);
}

HMAC::~HMAC()
{
    delete algo;
}

void HMAC::update(uint8_t *data, uint32_t length)
{
    algo->update(data, length);
}

void HMAC::finalize(uint8_t *hash)
{
    uint8_t buffer[64], buffer2[64];
    algo->finalize(buffer);
    
    memcpy(buffer2, key, keyLength);
    memset(&buffer2[keyLength], 0, 64-keyLength);
    for(int i = 0; i < 64; ++i)
        buffer2[i] ^= 0x5C;
        
    algo->update(buffer2, 64);
    algo->update(buffer, algo->outputSize());
    algo->finalize(hash);
}

