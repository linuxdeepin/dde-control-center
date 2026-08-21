// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cryptor.h"
#include <QDebug>
#include <openssl/err.h>
#include <openssl/rsa.h>

Cryptor::Cryptor()
{
}

bool Cryptor::RSAPublicEncryptData(const std::string &rsakey, const QString &strin, QByteArray &strout)
{
    BIO *bio = BIO_new_mem_buf(rsakey.c_str(), rsakey.length());
    EVP_PKEY *pkey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
    if(pkey == nullptr)
    {
        QString strerror = QString::fromLocal8Bit(ERR_error_string(ERR_get_error(), nullptr));
        qWarning() << "read rsa public key failed, error:" << strerror;
        qWarning() << "RSA pubkey:" << QString::fromStdString(rsakey);
        qWarning() << "length:" << rsakey.length();
        if(bio) BIO_free(bio);
        return false;
    }

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (!ctx || EVP_PKEY_encrypt_init(ctx) <= 0) {
        qWarning() << "Failed to initialize encryption context";
        if(ctx) EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        if(bio) BIO_free(bio);
        return false;
    }

    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0) {
        qWarning() << "Failed to set RSA padding";
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        if(bio) BIO_free(bio);
        return false;
    }

    QByteArray inputData = strin.toLocal8Bit();
    const unsigned char *input = (const unsigned char*)inputData.constData();
    size_t inlen = inputData.length();

    size_t outlen;
    if (EVP_PKEY_encrypt(ctx, nullptr, &outlen, input, inlen) <= 0) {
        qWarning() << "Failed to determine output length";
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        if(bio) BIO_free(bio);
        return false;
    }

    unsigned char *outbuff = new unsigned char[outlen];
    if (EVP_PKEY_encrypt(ctx, outbuff, &outlen, input, inlen) <= 0) {
        qWarning() << "Encryption failed";
        delete[] outbuff;
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        if(bio) BIO_free(bio);
        return false;
    }

    strout.append((char*)outbuff, outlen);
    delete[] outbuff;

    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    if(bio) BIO_free(bio);

    return true;
}
