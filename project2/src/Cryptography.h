#ifndef CRYPTOGRAPHY_H
#define CRYPTOGRAPHY_H

#include <crypto++/modes.h>

class Cryptography {
public:
    static void initialize(const int secret);
    
    static std::string encrypt(const std::string &plainMessage);
    static std::string decrypt(const std::string &encryptedMessage);
    
    static std::string createHMAC(const std::string &encodedMessage);
    static bool checkHMAC(const std::string &hmac, const std::string &encryptedMessage);
    
    static byte *m_iv, *m_key;
};

#endif