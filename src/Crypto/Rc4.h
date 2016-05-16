//
// Created by sen on 16-5-14.
//

#ifndef SOCKSTUNNELLOCAL_RC4_H
#define SOCKSTUNNELLOCAL_RC4_H

#include "Crypto.h"
#include <openssl/rc4.h>


class Rc4 : public Crypto
{
public:
    virtual void encrypt(String &clearText, const String &key);
    virtual void decrypt(String &chiperText, const String &key);
};


#endif //SOCKSTUNNELLOCAL_RC4_H
