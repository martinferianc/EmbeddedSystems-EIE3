#include "SHA384.h"


SHA384::SHA384():
HashAlgorithm(),
algo(SHA_384)
{
}

uint8_t SHA384::outputSize() const
{
    return 48;
}

void SHA384::update(uint8_t *data, uint32_t length)
{
    algo.update(data, length);
}

void SHA384::finalize(uint8_t *hash)
{
    algo.finalize(hash);
}

void SHA384::computeHash(uint8_t *hash, uint8_t *data, uint32_t length)
{
    SHA2_64::computeHash(SHA_384, hash, data, length);
}
