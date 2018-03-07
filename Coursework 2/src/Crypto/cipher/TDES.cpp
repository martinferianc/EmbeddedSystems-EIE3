#include "TDES.h"

TDES::TDES(uint8_t *key1, uint8_t *key2, uint8_t *key3):
BlockCipher(8,ECB_MODE),
a(key1),
b(key2),
c(key3)
{

}

TDES::TDES(uint8_t *key1, uint8_t *key2, uint8_t *key3, uint8_t *iv):
BlockCipher(8,CBC_MODE,iv),
a(key1),
b(key2),
c(key3)
{

}

void TDES::encryptBlock(uint8_t *out, uint8_t *in)
{
    uint8_t tmp[8], tmp2[8];
    a.encryptBlock(tmp,in);
    b.decryptBlock(tmp2,tmp);
    c.encryptBlock(out, tmp2);
}

void TDES::decryptBlock(uint8_t *out, uint8_t *in)
{
    uint8_t tmp[8], tmp2[8];
    c.decryptBlock(tmp, in);
    b.encryptBlock(tmp2, tmp);
    a.decryptBlock(out, tmp2);
}
