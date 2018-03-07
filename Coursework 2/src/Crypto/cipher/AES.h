#ifndef AES_H
#define AES_H

#include "BlockCipher.h"

enum AES_TYPE
{
    AES_128 = 4,
    AES_192 = 6,
    AES_256 = 8
};

class AES : public BlockCipher
{
    public :
    
        AES(const AES_TYPE type, uint8_t *key);
        AES(const AES_TYPE type, uint8_t *key, uint8_t *iv);
                
    private :
    
        virtual void encryptBlock(uint8_t *out, uint8_t *in);
        virtual void decryptBlock(uint8_t *out, uint8_t *in);
        
        void keyExpansion(uint8_t *key);
        uint32_t rotWord(uint32_t w);
        uint32_t invRotWord(uint32_t w);        
        uint32_t subWord(uint32_t w);
        void subBytes();
        void invSubBytes();
        void shiftRows();
        void invShiftRows();
        void mul(uint8_t *r);
        void invMul(uint8_t *r);
        void mixColumns();
        void invMixColumns();
        void addRoundKey(int round);

        uint8_t state[16];
        uint32_t w[60];
        uint8_t nr,nk;
};

#endif
