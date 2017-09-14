#ifndef CRYPTOGRAPHY_H
#define CRYPTOGRAPHY_H

#include <crypto++/modes.h>

class Cryptography {
public:
    static void initialize();
    
    static void encrypt(const std::string &plainText, std::string &encryptedText);
    static void decrypt(const std::string &encryptedText, std::string &decryptedText);
    
    static byte *m_iv, *m_key;
};

#endif