#ifndef CRYPTOBJECT_H
#define CRYPTOBJECT_H

class CryptObject
{
public:
    CryptObject();
    char *publickey_encode(char *sourcestr);
    char *publickey_decode(char *crypttext);
    char *privatekey_encode(char *sourcestr);
    char *privatekey_decode(char *crypttext);
    char *aes_encode(const char *sourcestr, char *key);
    char *aes_decode(const char *crypttext, char *key);
    char *base64_encode(const char* data, int data_len) ;
    char *base64_decode(const char* data, int data_len, int &out_len);

    char *getAeskey() const;
    void setAeskey(char *value);

private:
    char * aeskey;
};

#endif // CRYPTOBJECT_H
