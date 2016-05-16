//
// Created by sen on 16-5-14.
//

#include "CryptoHelper.h"

void CryptoHelper::encrypt(String &clearText, const String &key)
{
    for(auto &ptr : array_)
    {
        ptr->encrypt(clearText, key);
    }
}

void CryptoHelper::decrypt(String &chiperText, const String &key)
{
    for(auto it = array_.rbegin(); it != array_.rend(); ++it)
        (*it)->decrypt(chiperText, key);
}
