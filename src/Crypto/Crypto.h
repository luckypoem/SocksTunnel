//
// Created by sen on 16-5-14.
//

#ifndef SOCKSTUNNELLOCAL_CRYPTO_H
#define SOCKSTUNNELLOCAL_CRYPTO_H

#include <string>

typedef std::string String;

class Crypto
{
public:
    virtual ~Crypto() {}

public:
    virtual void encrypt(String &clearText, const String &key) = 0;
    virtual void decrypt(String &chiperText, const String &key) = 0;
};


#endif //SOCKSTUNNELLOCAL_CRYPTO_H
