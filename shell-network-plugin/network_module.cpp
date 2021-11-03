/*
* Copyright (C) 2021 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     Zhang Qipeng <zhangqipeng@uniontech.com>
*
* Maintainer: Zhang Qipeng <zhangqipeng@uniontech.com>
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

#include "network_module.h"
#include "networkpanel.h"

#include <QWidget>

#define NETWORK_KEY "network-item-key"

namespace dss {
namespace module {

NetworkModule::NetworkModule(QObject *parent)
    : QObject(parent)
    , m_networkPanel(nullptr)
{
    setObjectName(QStringLiteral(NETWORK_KEY));
    QTranslator *translator = new QTranslator(this);
    translator->load(QString("/usr/share/shell-network-plugin/translations/shell-network-plugin_%1.qm").arg(QLocale::system().name()));
    QCoreApplication::installTranslator(translator);
    initUI();
}

NetworkModule::~NetworkModule()
{
    if (m_networkPanel) {
        delete m_networkPanel;
    }
}

void NetworkModule::init()
{
    initUI();
}

void NetworkModule::initUI()
{
    if (m_networkPanel) {
        return;
    }
    m_networkPanel = new NetworkPanel();
    m_networkPanel->setAccessibleName(QStringLiteral("NetworkWidget"));
}

void NetworkModule::invokedMenuItem(const QString &itemKey, const QString &menuId, const bool checked) const
{
    Q_UNUSED(checked)

    if (itemKey == NETWORK_KEY)
         m_networkPanel->invokeMenuItem(menuId);
}

const QString NetworkModule::itemContextMenu(const QString &itemKey) const
{
    if (itemKey == NETWORK_KEY)
        return m_networkPanel->contextMenu();

    return QString();
}

QWidget *NetworkModule::itemWidget(const QString &itemKey) const
{
    if (itemKey == NETWORK_KEY)
        return m_networkPanel;

    return Q_NULLPTR;
}

QString NetworkModule::itemTips(const QString &itemKey) const
{
    if (itemKey == NETWORK_KEY)
        return m_networkPanel->itemTips();

    return QString();
}

QWidget *NetworkModule::content()
{
    m_networkPanel->onClick();
    return m_networkPanel;
}

} // namespace module
} // namespace dss
