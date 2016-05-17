//
// Created by sen on 16-5-17.
//

#include <string.h>
#include "Chacha20.h"

void Chacha20::encrypt(String &clearText, const String &key)
{
    uint8_t realKey[32] = {0};
    uint8_t nonce[8] = {0};
    strncpy((char *)realKey, key.c_str(), sizeof(realKey));
    strncpy((char *)nonce, key.c_str(), sizeof(nonce));
    Chacha20Codec codec(realKey, nonce, key.size());
    codec.crypt((uint8_t *)clearText.c_str(), clearText.size());
}

void Chacha20::decrypt(String &chiperText, const String &key)
{
    encrypt(chiperText, key);
}
