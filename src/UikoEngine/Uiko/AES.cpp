#include "AES.h"

#include <string.h>
#include <openssl/aes.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/err.h>

#pragma comment( lib, "libeay32.lib" )
#pragma comment( lib, "ssleay32.lib" )


static const unsigned char publicKey[] = "-----BEGIN PUBLIC KEY-----\n\
MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDCx71w8cYUiUxAu8Y6fFsK5fTe\n\
xktay7cvyEwP50eqPV3ot/3Gzvpnwkh3jy7MmvSjhBUuJFDfJB4zNBXXBjt22iNz\n\
pD8wq4jNvry9rbobngrC1DIEwXREeKqOBNLmhSxGw986+IVfjJxC1QDHGSVDwQ68\n\
i2t2K+f72gIDKhDrAQIDAQAB\n\
-----END PUBLIC KEY-----\n";
static const unsigned char privateKey[] = "-----BEGIN RSA PRIVATE KEY-----\n\
MIICXgIBAAKBgQDCx71w8cYUiUxAu8Y6fFsK5fTexktay7cvyEwP50eqPV3ot/3G\n\
zvpnwkh3jy7MmvSjhBUuJFDfJB4zNBXXBjt22iNzpD8wq4jNvry9rbobngrC1DIE\n\
wXREeKqOBNLmhSxGw986+IVfjJxC1QDHGSVDwQ68i2t2K+f72gIDKhDrAQIDAQAB\n\
AoGBAJDVrYw6e6vxm/3WosQCjEBCVh8BHC9bo8+oBMcely+Dijzt84rNhW16DiC5\n\
G3eREbA1GvrAjlIerUYtyL+nJrcfgwd7vEKSF/K9nVVBY8s90UFeUSXZr+aXqHVX\n\
JpLfCjlHO5BH8kIGi1P+KTge1/q6nY3PomlXC0agQMGKZZABAkEA+yNep0T6jsNS\n\
SkeiLJuzLX5dguqWdt7B0Bg82Q59idI483G3EoKLFOySA0VwbkwUQXymq6f5oskG\n\
7jc3UzLUoQJBAMaNEA6A5FFf3Ar+Tnei7tRgxvzWBf1PeRQ0XPt1hF/JoXbNrDmL\n\
gifkq//epjNpHZTrEqHA4DEtwxS6gHh2GmECQQDf0d4B5ZAIvDsSyFTYvMsruHtO\n\
uhPvTQgelftk120674iiLq6iaIXb2BspnzFcm6FR2+4O/l2VnuP2Wm+MTrkBAkB/\n\
vwbTrg44ZQq4cAKgas5rCpE8358e6eEnJMGmcGfpEVu3ioECzUXC0RfxGpmofjsw\n\
M0bbl7MnMSBQ1+YMm0BBAkEA5VGShHOP8SKW4bmHkznuDA3OTrT1V466xUATWOff\n\
AuVUuWWk7cyoSnszq/4Gv8TAS6161Le0tNoCdMN6vO14ow==\n\
-----END RSA PRIVATE KEY-----\n";
static const  char base[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
static char find_pos(char ch)
{
    char *ptr = (char*)strrchr(base, ch);//the last position (the only) in base[]
    return (ptr - base);
}

CryptObject::CryptObject()
{

}

char *CryptObject::publickey_encode(char *sourcestr)
{
    RSA *rsa = NULL;

    BIO *bio = NULL;
    if ((bio = BIO_new_mem_buf((void *)publicKey, -1)) == NULL) {
        fprintf(stderr, "Load Public Key Error!");
        return NULL;
    }

    rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
    if (!rsa) {
        ERR_load_crypto_strings();
        char errBuf[512];
        ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
        fprintf(stderr, "%s%s%s", "load public key failed[",errBuf,"]");
        BIO_free_all(bio);
        return NULL;
    }

    char * encrypted = (char *) malloc(1000+strlen(sourcestr) * 3);
    int len = RSA_public_encrypt(strlen(sourcestr), (unsigned char *)sourcestr, (unsigned char *)encrypted, rsa, RSA_PKCS1_PADDING);
    if (len == -1) {
        fprintf(stderr, "Failed to encrypt.\n");
        RSA_free(rsa);
        BIO_free_all(bio);
        return NULL;
    }
    char * result = base64_encode(encrypted, len);

    free(encrypted);
    RSA_free(rsa);
    BIO_free_all(bio);

    return result;
}

char *CryptObject::publickey_decode(char *crypttext)
{
    RSA *rsa = NULL;

    BIO *bio = NULL;
    if ((bio = BIO_new_mem_buf((void *)publicKey, -1)) == NULL) {
        fprintf(stderr, "Load Public Key Error!");
        return NULL;
    }

    rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
    if (!rsa) {
        ERR_load_crypto_strings();
        char errBuf[512];
        ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
        fprintf(stderr, "%s%s%s", "load public key failed[",errBuf,"]");
        BIO_free_all(bio);
        return NULL;
    }

    int outlen = 0;
    char *crypted = base64_decode(crypttext, strlen(crypttext), outlen);
    char * sourcestr = (char *) malloc(1000+strlen(crypted) * 3);
    int len = RSA_public_decrypt(outlen, (unsigned char *)crypted, (unsigned char *)sourcestr, rsa, RSA_PKCS1_PADDING);
    if (len == -1) {
        fprintf(stderr, "Failed to decrypt.\n");
        RSA_free(rsa);
        BIO_free_all(bio);
        return NULL;
    }
    sourcestr[len] = '\0';

    free(crypted);
    RSA_free(rsa);
    BIO_free_all(bio);

    return sourcestr;
}

char *CryptObject::privatekey_encode(char *sourcestr)
{
    RSA *rsa = NULL;

    BIO *bio = NULL;
    if ((bio = BIO_new_mem_buf((void *)privateKey, -1)) == NULL) {
        fprintf(stderr, "Load Private Key Error!");
        return NULL;
    }

    rsa = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL);
    if (!rsa) {
        ERR_load_crypto_strings();
        char errBuf[512];
        ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
        fprintf(stderr, "%s%s%s", "load private key failed[",errBuf,"]");
        BIO_free_all(bio);
        return NULL;
    }

    char * encrypted = (char *) malloc(1000+strlen(sourcestr) * 3);
    int len = RSA_private_encrypt(strlen(sourcestr), (unsigned char *)sourcestr, (unsigned char *)encrypted, rsa, RSA_PKCS1_PADDING);
    if (len == -1) {
        fprintf(stderr, "Failed to encrypt.\n");
        RSA_free(rsa);
        BIO_free_all(bio);
        return NULL;
    }
    char * result = base64_encode(encrypted, len);

    free(encrypted);
    RSA_free(rsa);
    BIO_free_all(bio);

    return result;
}

char *CryptObject::privatekey_decode(char *crypttext)
{
    RSA *rsa = NULL;

    BIO *bio = NULL;
    if ((bio = BIO_new_mem_buf((void *)privateKey, -1)) == NULL) {
        fprintf(stderr, "Load Private Key Error!");
        return NULL;
    }

    rsa = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL);
    if (!rsa) {
        ERR_load_crypto_strings();
        char errBuf[512];
        ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
        fprintf(stderr, "%s%s%s", "load private key failed[",errBuf,"]");
        BIO_free_all(bio);
        return NULL;
    }

    int outlen = 0;
    char *crypted = base64_decode(crypttext, strlen(crypttext), outlen);
    char * sourcestr = (char *) malloc(1000+strlen(crypted) * 3);
    int len = RSA_private_decrypt(outlen, (unsigned char *)crypted, (unsigned char *)sourcestr, rsa, RSA_PKCS1_PADDING);
    if (len == -1) {
        fprintf(stderr, "Failed to decrypt.\n");
        ERR_load_crypto_strings();
        char errBuf[512];
        ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
        fprintf(stderr, "%s\n", errBuf);
        RSA_free(rsa);
        BIO_free_all(bio);
        return NULL;
    }
    sourcestr[len] = '\0';

    free(crypted);
    RSA_free(rsa);
    BIO_free_all(bio);
    return sourcestr;
}

char *CryptObject::aes_encode(const char *sourcestr, char *key = "")
{
    if (strcmp(key, "") == 0) key = aeskey;

    int len = strlen(sourcestr);
    // unsigned char * iv= (unsigned char *)strcpy((char *)malloc(17), "6543210987654321");
    unsigned char iv[AES_BLOCK_SIZE+1] = "6543210987654321";

    unsigned char * out = (unsigned char *)malloc(1024*1024);
    if (out == NULL) {
        fprintf(stderr, "No Memory!\n");
    }
    AES_KEY aes;
    if(AES_set_encrypt_key((unsigned char*)key, 128, &aes) < 0)
    {
        return NULL;
    }
    int out_len = ((len - 1) / 16 + 1)* 16;
    char * sstr = (char *)malloc(sizeof(char) * out_len + 1);
    memset(sstr, 0, out_len+1);
	//strcpy(sstr, sourcestr);
	strcpy_s(sstr, strlen(sourcestr), sourcestr);
    AES_cbc_encrypt((unsigned char*)sstr, out, out_len, &aes, (unsigned char*)iv, AES_ENCRYPT);
    char * out2 = base64_encode((char *)out, out_len);
    free(out);
    free(sstr);
    return out2;
}

char *CryptObject::aes_decode(const char *crypttext, char *key = "")
{
    if (strcmp(key, "") == 0) key = aeskey;
    int out_len = 0;
    unsigned char iv[AES_BLOCK_SIZE+1] = "6543210987654321";

    //fprintf(stderr, "In:%s\n", crypttext);

    char *in = base64_decode(crypttext, strlen(crypttext), out_len);
    char *out = (char *) malloc(sizeof(char) * out_len + 1);
    memset(out, 0, out_len + 1);
    AES_KEY aes;
    if(AES_set_decrypt_key((unsigned char*)key, 128, &aes) < 0)
    {
        return NULL;
    }

    //fprintf(stderr, "Outlen:%d\n", out_len);
    AES_cbc_encrypt((unsigned char*)in, (unsigned char*)out, out_len, &aes, (unsigned char*)iv, AES_DECRYPT);
    //fprintf(stderr, "Output:%s\n", out);
    return out;
}

char *CryptObject::base64_encode(const char *data, int data_len)
{
    int prepare = 0;
        int ret_len;
        int temp = 0;
        char *ret = NULL;
        char *f = NULL;
        int tmp = 0;
        char changed[4];
        int i = 0;
        ret_len = data_len / 3;
        temp = data_len % 3;
        if (temp > 0)
        {
            ret_len += 1;
        }
        ret_len = ret_len*4 + 1;
        ret = (char *)malloc(ret_len);

        if ( ret == NULL)
        {
            printf("No enough memory.\n");
            exit(0);
        }
        memset(ret, 0, ret_len);
        f = ret;
        while (tmp < data_len)
        {
            temp = 0;
            prepare = 0;
            memset(changed, '\0', 4);
            while (temp < 3)
            {
                //printf("tmp = %d\n", tmp);
                if (tmp >= data_len)
                {
                    break;
                }
                prepare = ((prepare << 8) | (data[tmp] & 0xFF));
                tmp++;
                temp++;
            }
            prepare = (prepare<<((3-temp)*8));
            //printf("before for : temp = %d, prepare = %d\n", temp, prepare);
            for (i = 0; i < 4 ;i++ )
            {
                if (temp < i)
                {
                    changed[i] = 0x40;
                }
                else
                {
                    changed[i] = (prepare>>((3-i)*6)) & 0x3F;
                }
                *f = base[changed[i]];
                //printf("%.2X", changed[i]);
                f++;
            }
        }
        *f = '\0';

        return ret;
}

char *CryptObject::base64_decode(const char *data, int data_len, int &out_len)
{
    int ret_len = (data_len / 4) * 3;
        int equal_count = 0;
        char *ret = NULL;
        char *f = NULL;
        int tmp = 0;
        int temp = 0;
        char need[3];
        int prepare = 0;
        int i = 0;
        if (*(data + data_len - 1) == '=')
        {
            equal_count += 1;
        }
        if (*(data + data_len - 2) == '=')
        {
            equal_count += 1;
        }
        if (*(data + data_len - 3) == '=')
        {//seems impossible
            equal_count += 1;
        }
        switch (equal_count)
        {
        case 0:
            ret_len += 4;//3 + 1 [1 for NULL]
            break;
        case 1:
            ret_len += 4;//Ceil((6*3)/8)+1
            break;
        case 2:
            ret_len += 3;//Ceil((6*2)/8)+1
            break;
        case 3:
            ret_len += 2;//Ceil((6*1)/8)+1
            break;
        }
        ret = (char *)malloc(ret_len);
        if (ret == NULL)
        {
            printf("No enough memory.\n");
            exit(0);
        }
        memset(ret, 0, ret_len);
        f = ret;
        while (tmp < (data_len - equal_count))
        {
            temp = 0;
            prepare = 0;
            memset(need, 0, 3);
            while (temp < 4)
            {
                if (tmp >= (data_len - equal_count))
                {
                    break;
                }
                prepare = (prepare << 6) | (find_pos(data[tmp]));
                temp++;
                tmp++;
            }
            prepare = prepare << ((4-temp) * 6);
            for (i=0; i<3 ;i++ )
            {
                if (i == temp)
                {
                    break;
                }
                *f = (char)((prepare>>((2-i)*8)) & 0xFF);
                f++;
            }
        }
        *f = '\0';
        out_len = (int)(f - ret);
        if (out_len < 0) out_len = 0;
        return ret;
}
char *CryptObject::getAeskey() const
{
    return aeskey;
}

void CryptObject::setAeskey(char *value)
{
    aeskey = value;
}

