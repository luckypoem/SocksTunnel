//
// Created by sen on 16-5-14.
//

#include "Rc4.h"

void Rc4::encrypt(String &clearText, const String &key)
{
    RC4_KEY rc4key;
    char* tmp = new char[clearText.size() + 1];

    RC4_set_key(&rc4key, key.size(), (const unsigned char*)key.c_str());
    RC4(&rc4key, clearText.size(), (const unsigned char*)clearText.c_str(), (unsigned char *)tmp);

    clearText.assign(tmp, clearText.size());

    delete [] tmp;
}

void Rc4::decrypt(String &chiperText, const String &key)
{
    RC4_KEY rc4key;
    char* tmp = new char[chiperText.size() + 1];

    RC4_set_key(&rc4key, key.size(), (const unsigned char*)key.c_str());
    RC4(&rc4key, chiperText.size(), (const unsigned char*)chiperText.c_str(), (unsigned char *)tmp);

    chiperText.assign(tmp, chiperText.size());
    delete [] tmp;
}
