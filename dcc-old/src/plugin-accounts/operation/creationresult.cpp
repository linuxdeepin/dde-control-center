//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "creationresult.h"


using namespace DCC_NAMESPACE;

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

