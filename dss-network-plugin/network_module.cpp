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
    translator->load(QString("/usr/share/dss-network-plugin/translations/dss-network-plugin_%1.qm").arg(QLocale::system().name()));
    QCoreApplication::installTranslator(translator);
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

QWidget *NetworkModule::content()
{
    m_networkPanel->onClick();
    return nullptr;
}

QWidget *NetworkModule::itemWidget() const
{
    return m_networkPanel;
}

QWidget *NetworkModule::itemTipsWidget() const
{
    return m_networkPanel->itemTips();
}

const QString NetworkModule::itemContextMenu() const
{
    return m_networkPanel->contextMenu();
}

void NetworkModule::invokedMenuItem(const QString &menuId, const bool checked) const
{
    Q_UNUSED(checked);

    m_networkPanel->invokeMenuItem(menuId);
}

void NetworkModule::initUI()
{
    if (m_networkPanel) {
        return;
    }
    m_networkPanel = new NetworkPanel();
    m_networkPanel->setAccessibleName(QStringLiteral("NetworkPanel"));
}

} // namespace module
} // namespace dss
