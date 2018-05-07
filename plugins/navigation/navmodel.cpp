/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <190771752ri@gmail.com>
 *
 * Maintainer: listenerri <190771752ri@gmail.com>
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

#include "navmodel.h"

NavModel::NavModel(QObject *parent) : QAbstractTableModel(parent)
{
    m_bluetoothInter = new BluetoothInter("com.deepin.daemon.Bluetooth",
                                          "/com/deepin/daemon/Bluetooth",
                                          QDBusConnection::sessionBus(), this);
    m_wacomInter = new WacomInter("com.deepin.daemon.InputDevices",
                                  "/com/deepin/daemon/InputDevice/Wacom",
                                  QDBusConnection::sessionBus(), this);

    m_moduleList = validModuleList();
    m_hoverIndex = QModelIndex();

    connect(m_bluetoothInter, &BluetoothInter::StateChanged, this, &NavModel::onBTStateChanged);
    connect(m_wacomInter, &WacomInter::ExistChanged, this, &NavModel::onWacomExistChanged);
}

int NavModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    int mSize = m_moduleList.size();
    return mSize % 3 > 0 ? mSize / 3 + 1 : mSize / 3;
}

int NavModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return 3;
}

QVariant NavModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    int mIndex = index.row() * 3 + index.column();
    if (mIndex >= m_moduleList.size()) {
        return QVariant();
    }

    switch (role)
    {
    case Qt::WhatsThisRole:
        return m_moduleList.at(mIndex);
        break;
    case NavHoverRole:
        return m_hoverIndex == index;
        break;
    default:;
    }

    return QVariant();
}

void NavModel::removeModule(const QModelIndex &index)
{
    removeModule(data(index, Qt::WhatsThisRole).toString());
}

void NavModel::removeModule(const QString &moduleName)
{
    if (!m_moduleList.contains(moduleName)) {
        return;
    }

    m_moduleList.removeOne(moduleName);
    Q_EMIT layoutChanged();
}

void NavModel::addModule(const QString &moduleName)
{
    if (m_moduleList.contains(moduleName) || !validModuleList().contains(moduleName)) {
        return;
    }

    int mIndex = 0;
    for (int i = 0; i < MODULES.size(); ++i) {
        if (MODULES.at(i) == moduleName)
            break;
        if (m_moduleList.contains(MODULES.at(i)))
            mIndex++;
    }
    m_moduleList.insert(mIndex, moduleName);
    Q_EMIT layoutChanged();
}

void NavModel::setHoverIndex(const QModelIndex &index)
{
    m_hoverIndex = index;
    Q_EMIT dataChanged(m_hoverIndex, m_hoverIndex);
}

QString NavModel::transModuleName(const QString &moduleName)
{
    static const QStringList modules_trans = {
        QT_TRANSLATE_NOOP("dcc::accounts::AccountsWidget", "Accounts"),
        QT_TRANSLATE_NOOP("dcc::display::DisplayWidget", "Display"),
        QT_TRANSLATE_NOOP("dcc::defapp::DefAppViewer", "Default Applications"),
        QT_TRANSLATE_NOOP("dcc::personalization::PersonalizationWidget", "Personalization"),
        QT_TRANSLATE_NOOP("dcc::network::NetworkModuleWidget", "Network"),
        QT_TRANSLATE_NOOP("dcc::bluetooth::BluetoothModule", "Bluetooth"),
        QT_TRANSLATE_NOOP("dcc::sound::SoundWidget", "Sound"),
        QT_TRANSLATE_NOOP("dcc::datetime::Datetime", "Time and Date"),
        QT_TRANSLATE_NOOP("dcc::power::PowerWidget", "Power Management"),
        QT_TRANSLATE_NOOP("dcc::mouse::MouseWidget", "Mouse"),
        QT_TRANSLATE_NOOP("dcc::keyboard::KeyboardWidget", "Keyboard and Language"),
        QT_TRANSLATE_NOOP("dcc::wacom::WacomWidget", "Wacom"),
        QT_TRANSLATE_NOOP("dcc::update::UpdateCtrlWidget", "Update"),
        QT_TRANSLATE_NOOP("dcc::systeminfo::SystemInfoWidget", "System Information"),
    };

    static const QStringList modules_scope = {
        "dcc::accounts::AccountsWidget",
        "dcc::display::DisplayWidget",
        "dcc::defapp::DefAppViewer",
        "dcc::personalization::PersonalizationWidget",
        "dcc::network::NetworkModuleWidget",
        "dcc::bluetooth::BluetoothModule",
        "dcc::sound::SoundWidget",
        "dcc::datetime::Datetime",
        "dcc::power::PowerWidget",
        "dcc::mouse::MouseWidget",
        "dcc::keyboard::KeyboardWidget",
        "dcc::wacom::WacomWidget",
        "dcc::update::UpdateCtrlWidget",
        "dcc::systeminfo::SystemInfoWidget",
    };

    const int idx = moduleName.isEmpty() ? -1 : MODULES.indexOf(moduleName);

    if (idx == -1)
        return QString("Navigation");
    else
        return QCoreApplication::translate(modules_scope[idx].toStdString().c_str(),
                                           modules_trans[idx].toStdString().c_str());
}

void NavModel::onBTStateChanged(uint value)
{
    if (value > 0) {
        addModule("bluetooth");
        return;
    }
    removeModule("bluetooth");
}

void NavModel::onWacomExistChanged(bool value)
{
    if (value) {
        addModule("wacom");
        return;
    }
    removeModule("wacom");
}

QStringList NavModel::validModuleList()
{
    QStringList moduleList = MODULES;

#ifdef DISABLE_ACCOUNT
    moduleList.removeOne("accounts");
#endif

#ifdef DISABLE_DISPALY
    moduleList.removeOne("display");
#endif

#ifdef DISABLE_DEFAULT_APPLICATIONS
    moduleList.removeOne("defapp");
#endif

#ifdef DISABLE_PERSONALIZATION
    moduleList.removeOne("personalization");
#endif

#ifdef DISABLE_NETWORK
    moduleList.removeOne("network");
#endif

#ifdef DISABLE_SOUND
    moduleList.removeOne("sound");
#endif

#ifdef DISABLE_DATETIME
    moduleList.removeOne("datetime");
#endif

#ifdef DISABLE_POWER
    moduleList.removeOne("power");
#endif

#ifdef DISABLE_MOUSE
    moduleList.removeOne("mouse");
#endif

#ifdef DISABLE_KAYBOARD
    moduleList.removeOne("keyboard");
#endif

#ifdef DISABLE_SYS_UPDATE
    moduleList.removeOne("update");
#endif

#ifndef DISABLE_BLUETOOTH
    if (!m_bluetoothInter->state())
#endif
        moduleList.removeOne("bluetooth");

#ifndef DISABLE_WACOM
    if (!m_wacomInter->exist())
#endif
        moduleList.removeOne("wacom");

    return moduleList;
}
