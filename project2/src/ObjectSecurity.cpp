#include <iostream>

#include <crypto++/modes.h>
#include <crypto++/aes.h>
#include <crypto++/filters.h>

using namespace std;

byte g_key[CryptoPP::AES::DEFAULT_KEYLENGTH], g_iv[CryptoPP::AES::BLOCKSIZE];

string encrypt(const string &plain) {    
    CryptoPP::AES::Encryption aesEncryption(g_key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, g_iv);
    
    string cipherText;
    
    CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(cipherText));
    stfEncryptor.Put(reinterpret_cast<const unsigned char*>(plain.c_str()), plain.length() + 1);
    stfEncryptor.MessageEnd();
    
    return cipherText;
}

string decrypt(const string &encrypted) {    
    CryptoPP::AES::Decryption aesDecryption(g_key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, g_iv);
    
    string decryptedText;
        
    CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decryptedText));
    stfDecryptor.Put(reinterpret_cast<const unsigned char*>(encrypted.c_str()), encrypted.size());
    stfDecryptor.MessageEnd();
    
    return decryptedText;
}

int main() {
    memset(g_key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH);
    memset(g_iv, 0x00, CryptoPP::AES::BLOCKSIZE);
    
    string encrypted = encrypt("hej magnus");
    string decrypted = decrypt(encrypted);
    
    cout << "Encrypted text: " << encrypted << endl;
    cout << "Decrypted text: " << decrypted << endl;
    
    return 0;
}