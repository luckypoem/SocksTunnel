//
// Created by sen on 16-5-17.
//

#ifndef SOCKSTUNNELLOCAL_CHACHA20_H
#define SOCKSTUNNELLOCAL_CHACHA20_H

#include "chacha20Codec.hpp"
#include "Crypto.h"

class Chacha20 : public Crypto
{

public:
    virtual void encrypt(String &clearText, const String &key);
    virtual void decrypt(String &chiperText, const String &key);
};


#endif //SOCKSTUNNELLOCAL_CHACHA20_H
