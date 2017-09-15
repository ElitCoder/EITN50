#include "Cryptography.h"

#include <iostream>

#include <crypto++/aes.h>
#include <crypto++/filters.h>
#include <crypto++/hmac.h>
#include <crypto++/sha.h>

using namespace std;

byte *Cryptography::m_iv = new byte[CryptoPP::AES::BLOCKSIZE];
byte *Cryptography::m_key = new byte[CryptoPP::AES::DEFAULT_KEYLENGTH];

string Cryptography::createHMAC(const string &encodedMessage) {
    using namespace CryptoPP;
    
    HMAC<SHA256> hmac(m_iv, AES::BLOCKSIZE);
    
    string mac;
    StringSource(encodedMessage, true, new HashFilter(hmac, new StringSink(mac)));
    
    cout << "<CRYPTO> Created HMAC from AES-encrypted message with size " << mac.length() * 8 << endl;
    
    return mac;
}

bool Cryptography::checkHMAC(const string &hmac, const string &encryptedMessage) {
    auto result = hmac == createHMAC(encryptedMessage);
    
    cout << "<CRYPTO> Checking HMAC.. " << (result ? "matching!" : "not matching!") << endl;
    
    return result;
}

void Cryptography::initialize(const int secret) {
    memset(m_key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH);
    memset(m_iv, 0x00, CryptoPP::AES::BLOCKSIZE);
    
    cout << "<CRYPTO> Creating IV with information from DH shared key\n";

    m_iv[0] = (secret >> 24) & 0xFF;
    m_iv[1] = (secret >> 16) & 0xFF;
    m_iv[2] = (secret >> 8) & 0xFF;
    m_iv[3] = secret & 0xFF;
}

string Cryptography::encrypt(const string &plainText) {
    cout << "<CRYPTO> Encrypting message using AES, message: " << plainText << endl;
    
    CryptoPP::AES::Encryption aesEncryption(m_key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, m_iv);
    
    string encryptedText;
    CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(encryptedText));
    
    stfEncryptor.Put(reinterpret_cast<const unsigned char*>(plainText.c_str()), plainText.length() + 1);
    stfEncryptor.MessageEnd();
    
    return encryptedText;
}

string Cryptography::decrypt(const string &encryptedText) {
    cout << "<CRYPTO> Decrypting message using AES\n";
    
    CryptoPP::AES::Decryption aesDecryption(m_key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, m_iv);

    string decryptedText;
    CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decryptedText));
    
    stfDecryptor.Put(reinterpret_cast<const unsigned char*>(encryptedText.c_str()), encryptedText.size());
    stfDecryptor.MessageEnd();    
    
    return decryptedText;
}