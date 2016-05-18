#include "Chacha20.h"
#include <string>
#include <iostream>

using namespace std;

int main()
{
    Chacha20 cc;
    std::string word = "this is a test";
    cc.encrypt(word, "hellowosdsaddddddddddddddddddddddddddddddddddddddddddddddddd");
    cout << word << endl;
    cc.decrypt(word, "hellowosdsaddddddddddddddddddddddddddddddddddddddddddddddddd");
    cout << word << endl;
}