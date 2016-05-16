//
// Created by sen on 16-5-16.
//

#include "Xor.h"

void Xor::encrypt(String &clearText, const String &key)
{
    for(auto &i : clearText)
        i ^= (~0);
}

void Xor::decrypt(String &chiperText, const String &key)
{
    for(auto &i : chiperText)
        i ^= (~0);
}
