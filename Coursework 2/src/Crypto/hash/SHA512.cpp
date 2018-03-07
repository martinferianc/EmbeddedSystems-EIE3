#include "SHA512.h"


SHA512::SHA512():
HashAlgorithm(),
algo(SHA_512)
{
}

uint8_t SHA512::outputSize() const
{
    return 64;
}

void SHA512::update(uint8_t *data, uint32_t length)
{
    algo.update(data, length);
}

void SHA512::finalize(uint8_t *hash)
{
    algo.finalize(hash);
}

void SHA512::computeHash(uint8_t *hash, uint8_t *data, uint32_t length)
{
    SHA2_64::computeHash(SHA_512, hash, data, length);
}
