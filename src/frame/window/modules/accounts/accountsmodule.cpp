/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
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

#include "accountsmodule.h"

using namespace DCC_NAMESPACE::accounts;

AccountsModule::AccountsModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_accountsWidget(nullptr)
{

}

void AccountsModule::initialize()
{
    m_userList = new UserModel;
    m_accountsWorker = new AccountsWorker(m_userList);

    m_accountsWorker->moveToThread(qApp->thread());
    m_userList->moveToThread(qApp->thread());

    m_accountsWorker->active();
}

void AccountsModule::reset()
{

}

const QString AccountsModule::name() const
{
    return QStringLiteral("accounts");
}

void AccountsModule::showPage(const QString &pageName)
{
    Q_UNUSED(pageName);
}

QWidget *AccountsModule::moduleWidget()
{
    if (!m_accountsWidget) {
        m_accountsWidget = new AccountsWidget;
        m_accountsWidget->setModel(m_userList);
    }

    return m_accountsWidget;
}

void AccountsModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w)
}



AccountsModule::~AccountsModule()
{
    m_userList->deleteLater();
    m_accountsWorker->deleteLater();
}
