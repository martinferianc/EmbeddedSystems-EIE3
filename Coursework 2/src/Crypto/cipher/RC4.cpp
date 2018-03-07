#include "RC4.h"

RC4::RC4(uint8_t *key, uint8_t keyLength):
StreamCipher(),
s(),
i(0),
j(0)
{
    for(int k = 0; k < 256; ++k)
        s[k] = k;
    int l = 0;
    for(int k = 0; k < 256; ++k)
    {
        l = (l + s[k] + key[k % keyLength]) % 256;
        uint8_t tmp = s[l];
        s[l] = s[k];
        s[k] = tmp;
    } 
}

uint8_t RC4::encryptByte(uint8_t in)
{
    ++i;
    j += s[i];
    uint8_t tmp = s[i];
    s[i] = s[j];
    s[j] = tmp;
    uint8_t c = s[(s[i]+s[j])%256];
    return in^c;  
}

uint8_t RC4::decryptByte(uint8_t in)
{
    return encryptByte(in);
}
