#include "SHA256.h"


SHA256::SHA256():
HashAlgorithm(),
algo(SHA_256)
{
}

uint8_t SHA256::outputSize() const
{
    return 32;
}

void SHA256::update(uint8_t *data, uint32_t length)
{
    algo.update(data, length);
}

void SHA256::finalize(uint8_t *hash)
{
    algo.finalize(hash);
}

void SHA256::computeHash(uint8_t *hash, uint8_t *data, uint32_t length)
{
    SHA2_32::computeHash(SHA_256, hash, data, length);
}
