/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
