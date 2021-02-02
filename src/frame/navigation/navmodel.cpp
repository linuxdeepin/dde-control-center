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

#include "navmodel.h"

#include <DStyleOption>

#include <QColor>
#include <QCoreApplication>
#include <QDebug>
#include <QIcon>

NavModel::NavModel(int columnCount, QObject *parent)
    : QAbstractTableModel(parent)
    , m_columnCount(columnCount)
{
    m_moduleList = validModuleList();
    m_hoverIndex = QModelIndex();
    initModuleTypeMapData();

    setModuleVisible("bluetooth", false);
    setModuleVisible("wacom", false);
    setModuleVisible("Cloud Sync", false);
}

int NavModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    int mSize = m_moduleList.size();
    // 最后一行在未完全填充时多出的元素个数
    int last_extra_count = mSize % m_columnCount;

    return last_extra_count > 0 ? mSize / m_columnCount + last_extra_count : mSize / m_columnCount;
}

int NavModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_columnCount;
}

QVariant NavModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    int mIndex = index.row() * m_columnCount + index.column();
    if (mIndex >= m_moduleList.size()) {
        return QVariant();
    }

    switch (static_cast<int>(role))
    {
    case Qt::WhatsThisRole:
        return m_moduleList.at(mIndex);
        break;
    case NavHoverRole:
        return m_hoverIndex == index;
        break;
    case Qt::DisplayRole:
    case NavDisplayRole:
        return transModuleName(m_moduleList.at(mIndex));
        break;
    case Qt::DecorationRole:
        return QIcon(QString(":/%1/themes/dark/icons/nav_%1.svg").arg(m_moduleList.at(mIndex)));
    case NavModuleType:
        return m_moduleTypeMap.value(m_moduleList.at(mIndex));
    default:;
    }

    return QVariant();
}

void NavModel::initModuleTypeMapData()
{
    m_moduleTypeMap.insert(MODULES.at(0), AccountsModule);
    m_moduleTypeMap.insert(MODULES.at(1), Cloudsync);
    m_moduleTypeMap.insert(MODULES.at(2), Display);
    m_moduleTypeMap.insert(MODULES.at(3), Defapp);
    m_moduleTypeMap.insert(MODULES.at(4), Personalization);
    m_moduleTypeMap.insert(MODULES.at(5), Network);
    m_moduleTypeMap.insert(MODULES.at(6), Bluetooth);
    m_moduleTypeMap.insert(MODULES.at(7), Sound);
    m_moduleTypeMap.insert(MODULES.at(8), Datetime);
    m_moduleTypeMap.insert(MODULES.at(9), Power);
    m_moduleTypeMap.insert(MODULES.at(10), Mouse);
    m_moduleTypeMap.insert(MODULES.at(11), Keyboard);
    m_moduleTypeMap.insert(MODULES.at(12), Wacom);
    m_moduleTypeMap.insert(MODULES.at(13), Update);
    m_moduleTypeMap.insert(MODULES.at(14), Systeminfo);
}

void NavModel::setHoverIndex(const QModelIndex &index)
{
    m_hoverIndex = index;
    Q_EMIT dataChanged(m_hoverIndex, m_hoverIndex);
}

void NavModel::setModuleVisible(const QString &module, bool visible)
{
    qDebug() << module;
    if (visible) {
        m_hideModuleList.removeOne(module);
    }
    else {
        if (!m_hideModuleList.contains(module)) {
            m_hideModuleList << module;
        }
    }

    m_moduleList = validModuleList();

    for (auto it = m_moduleList.begin(); it != m_moduleList.end();) {
        if (m_hideModuleList.contains(*it)) {
            it = m_moduleList.erase(it);
        }
        else {
            ++it;
        }
    }

    Q_EMIT layoutChanged();
}

QString NavModel::transModuleName(const QString &moduleName) const
{
    static const QStringList modules_trans = {
        QT_TRANSLATE_NOOP("dcc::accounts::AccountsWidget", "Accounts"),
        QT_TRANSLATE_NOOP("dcc::cloudsync::SyncWidget", "Cloud Sync"),
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
        "dcc::cloudsync::SyncWidget",
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
        return tr("Navigation");
    else
        return QCoreApplication::translate(modules_scope[idx].toStdString().c_str(),
                                           modules_trans[idx].toStdString().c_str());
}

QStringList NavModel::validModuleList()
{
    QStringList moduleList = MODULES;

#ifdef DISABLE_ACCOUNT
    moduleList.removeOne("accounts");
#endif

#ifdef DISABLE_CLOUD_SYNC
    moduleList.removeOne("cloudsync");
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

#ifdef DISABLE_SYSINFO
    moduleList.removeOne("systeminfo");
#endif

    return moduleList;
}
