#include "Cryptography.h"

#include <crypto++/aes.h>
#include <crypto++/filters.h>

using namespace std;

byte *Cryptography::m_iv = new byte[CryptoPP::AES::BLOCKSIZE];
byte *Cryptography::m_key = new byte[CryptoPP::AES::DEFAULT_KEYLENGTH];

void Cryptography::initialize() {
    memset(m_key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH);
    memset(m_iv, 0x00, CryptoPP::AES::BLOCKSIZE);
}

void Cryptography::encrypt(const string &plainText, string &encryptedText) {
    CryptoPP::AES::Encryption aesEncryption(m_key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, m_iv);
        
    CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(encryptedText));
    stfEncryptor.Put(reinterpret_cast<const unsigned char*>(plainText.c_str()), plainText.length() + 1);
    stfEncryptor.MessageEnd();    
}

void Cryptography::decrypt(const string &encryptedText, string &decryptedText) {
    CryptoPP::AES::Decryption aesDecryption(m_key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, m_iv);
            
    CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decryptedText));
    stfDecryptor.Put(reinterpret_cast<const unsigned char*>(encryptedText.c_str()), encryptedText.size());
    stfDecryptor.MessageEnd();    
}