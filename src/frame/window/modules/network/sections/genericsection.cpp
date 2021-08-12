/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
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

#include "genericsection.h"
#include "window/gsettingwatcher.h"

#include <networkmanagerqt/settings.h>

#include <QLineEdit>

using namespace NetworkManager;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::network;

GenericSection::GenericSection(NetworkManager::ConnectionSettings::Ptr connSettings, QFrame *parent)
    : AbstractSection(tr("General"), parent)
    , m_connIdItem(new LineEditWidget(this))
    , m_autoConnItem(new SwitchWidget(this))
    , m_connSettings(connSettings)
    , m_connType(NetworkManager::ConnectionSettings::Unknown)
{
    initUI();
    m_connIdItem->textEdit()->installEventFilter(this);
    connect(m_autoConnItem, &SwitchWidget::checkedChanged, this, &GenericSection::editClicked);
}

GenericSection::~GenericSection()
{
    delete m_connIdItem;
    m_connIdItem = nullptr;
    delete m_autoConnItem;
    m_autoConnItem = nullptr;
}

void GenericSection::setConnectionType(NetworkManager::ConnectionSettings::ConnectionType connType)
{
    m_connType = connType;
}

bool GenericSection::allInputValid()
{
    bool valid = true;
    QString inputTxt = m_connIdItem->textEdit()->text();
    if (inputTxt.isEmpty()) {
        m_connIdItem->setIsErr(true);
        return false;
    } else {
        if (m_connType == NetworkManager::ConnectionSettings::Vpn) {
            NetworkManager::Connection::List connList = listConnections();
            QStringList connNameList;
            QString curUuid = "";
            if (!m_connSettings.isNull()) {
                curUuid = m_connSettings->uuid();
            }
            for (auto conn : connList) {
                if (conn->settings()->connectionType() == m_connType) {
                    if ((conn->name() == inputTxt) && (conn->uuid() != curUuid)) {
                        m_connIdItem->setIsErr(true);
                        m_connIdItem->dTextEdit()->showAlertMessage(tr("The name already exists"), m_connIdItem, 2000);
                        return false;
                    }
                }
            }
        }
    }

    return valid;
}

void GenericSection::saveSettings()
{
    m_connSettings->setId(m_connIdItem->text());
    m_connSettings->setAutoconnect(m_autoConnItem->checked());
}

bool GenericSection::autoConnectChecked() const
{
    return m_autoConnItem->checked();
}

void GenericSection::setConnectionNameEditable(const bool editable)
{
    m_connIdItem->textEdit()->setClearButtonEnabled(editable);
    m_connIdItem->textEdit()->setEnabled(editable);
}

void GenericSection::initUI()
{
    setAccessibleName("GenericSection");
    m_connIdItem->setTitle(tr("Name"));
    m_connIdItem->setText(m_connSettings->id());
    m_autoConnItem->setChecked(m_connSettings->autoconnect());
    m_autoConnItem->setTitle(tr("Auto Connect"));

    appendItem(m_connIdItem);
    appendItem(m_autoConnItem);
}

bool GenericSection::eventFilter(QObject *watched, QEvent *event)
{
    // 实现鼠标点击编辑框，确定按钮激活，统一网络模块处理，捕捉FocusIn消息
    if (event->type() == QEvent::FocusIn && dynamic_cast<QLineEdit*>(watched)) {
        Q_EMIT editClicked();
    }
    return QWidget::eventFilter(watched, event);
}
