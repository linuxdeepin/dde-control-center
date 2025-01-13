// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cryptor.h"
#include <QDebug>
#include <openssl/err.h>

Cryptor::Cryptor()
{
}

bool Cryptor::RSAPublicEncryptData(const std::string &rsakey, const QString &strin, QByteArray &strout)
{
    RSA *rsa = nullptr;
    BIO *bio = BIO_new_mem_buf(rsakey.c_str(), rsakey.length());
    rsa = PEM_read_bio_RSA_PUBKEY(bio, &rsa, nullptr, nullptr);// PEM_read_bio_RSAPublicKey(bio, &rsa, nullptr, nullptr);
    if(rsa == nullptr)
    {
        QString strerror = QString::fromLocal8Bit(ERR_error_string(ERR_get_error(), nullptr));
        qWarning() << "read rsa public key failed, error:" << strerror;
        qWarning() << "RSA pubkey:" << QString::fromStdString(rsakey);
        qWarning() << "length:" << rsakey.length();
        return false;
    }

    int length = RSA_size(rsa);
    unsigned char *outbuff = new unsigned char[length];
    memset(outbuff, 0, length);
    int ret = RSA_public_encrypt(strin.length(), (const unsigned char*)strin.toLocal8Bit().data(), outbuff, rsa, RSA_PKCS1_PADDING);
    //strout = QString::fromLocal8Bit((char*)outbuff, length + 1);
    strout.append((char*)outbuff, length);
    delete [] outbuff;
    outbuff = nullptr;
    if(bio)
    {
        BIO_free(bio);
    }

    if(rsa)
    {
        RSA_free(rsa);
    }

    return true;
}
