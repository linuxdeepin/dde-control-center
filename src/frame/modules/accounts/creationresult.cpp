// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "creationresult.h"

namespace dcc {
namespace accounts {

CreationResult::CreationResult(QObject *parent)
    : CreationResult(NoError, QString(""), parent)
{

}

CreationResult::CreationResult(CreationResult::ResultType type, const QString &message, QObject *parent)
    : QObject(parent)
    , m_type(type)
    , m_message(message)
{

}

void CreationResult::setType(const ResultType &type)
{
    m_type = type;
}

void CreationResult::setMessage(const QString &message)
{
    m_message = message;
}

} // namespace accounts
} // namespace dcc
