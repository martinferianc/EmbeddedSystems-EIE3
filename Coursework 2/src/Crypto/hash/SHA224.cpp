#include "SHA224.h"


SHA224::SHA224():
HashAlgorithm(),
algo(SHA_224)
{
}

uint8_t SHA224::outputSize() const
{
    return 28;
}

void SHA224::update(uint8_t *data, uint32_t length)
{
    algo.update(data, length);
}

void SHA224::finalize(uint8_t *hash)
{
    algo.finalize(hash);
}

void SHA224::computeHash(uint8_t *hash, uint8_t *data, uint32_t length)
{
    SHA2_32::computeHash(SHA_224, hash, data, length);
}
