//
// Created by sen on 16-5-14.
//

#ifndef SOCKSTUNNELLOCAL_CRYPTOHELPER_H
#define SOCKSTUNNELLOCAL_CRYPTOHELPER_H


#include "../CommonLib/Singleton.h"
#include "Crypto.h"
#include "../CommonLib/Logger.h"
#include <vector>
#include "../CommonLib/SharedPtr.h"

using namespace loggable::debugger;

typedef ptr::SharedPtr<Crypto> CryptoWrap;
typedef std::vector<CryptoWrap> CryptoArray;

class CryptoHelper : public utils::ISingleton<CryptoHelper>, public Crypto
{
public:
    CryptoHelper() {  }
    ~CryptoHelper() { }
public:
    void pushCrypto(const CryptoWrap &cptr) { array_.push_back(cptr); }
    CryptoWrap popCrypto() { CryptoWrap ptr = array_.back(); array_.pop_back(); return ptr; }
    void encrypt(String &clearText, const String &key);
    void decrypt(String &chiperText, const String &key);

private:
    CryptoArray array_;
};


#endif //SOCKSTUNNELLOCAL_CRYPTOHELPER_H
