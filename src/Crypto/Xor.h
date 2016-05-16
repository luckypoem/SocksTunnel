//
// Created by sen on 16-5-16.
//

#ifndef SOCKSTUNNELLOCAL_XOR_H
#define SOCKSTUNNELLOCAL_XOR_H


#include "Crypto.h"

class Xor : public Crypto
{
public:
    void encrypt(String &clearText, const String &key);
    void decrypt(String &chiperText, const String &key);
};


#endif //SOCKSTUNNELLOCAL_XOR_H
