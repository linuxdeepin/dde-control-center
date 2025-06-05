// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CRYPTOR_H
#define CRYPTOR_H
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <QString>

class Cryptor
{
public:
    Cryptor();

    static bool RSAPublicEncryptData(const std::string &rsakey, const QString &strin, QByteArray &strout);
};

#endif // CRYPTOR_H
