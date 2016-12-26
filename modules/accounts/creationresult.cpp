/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "creationresult.h"

namespace dcc {
namespace accounts {

CreationResult::CreationResult(QObject *parent)
    : CreationResult(NoError, QString(""), parent)
{

}

CreationResult::CreationResult(CreationResult::ResultType type, QString message, QObject *parent)
    : QObject(parent),
      m_type(type),
      m_message(message)
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
