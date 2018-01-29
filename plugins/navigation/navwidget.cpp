/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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

#include "navwidget.h"
#include "navitemwidget.h"

#include <QTimer>

const QStringList MODULES = { "accounts", "display", "defapp", "personalization", "network",
                              "bluetooth", "sound", "datetime", "power", "mouse", "keyboard",
                              "wacom", "update", "systeminfo" };

NavWidget::NavWidget(QWidget *parent)
    : QWidget(parent),

      m_deviceRefreshDelay(new QTimer(this)),
      m_bluetoothInter(new BluetoothInter("com.deepin.daemon.Bluetooth", "/com/deepin/daemon/Bluetooth", QDBusConnection::sessionBus(), this)),
      m_wacomInter(new WacomInter("com.deepin.daemon.InputDevices", "/com/deepin/daemon/InputDevice/Wacom", QDBusConnection::sessionBus(), this))
{
    m_tipsLabel = new QLabel;
    m_tipsLabel->setStyleSheet("QLabel {"
                               "color: white;"
                               "background-color: rgba(255, 255, 255, .03);"
                               "padding: 4px 0;"
                               "}");
    m_tipsLabel->setAlignment(Qt::AlignCenter);
    m_gridLayout = new QGridLayout;
    m_gridLayout->setSpacing(1);
    m_gridLayout->setMargin(0);
    m_deviceRefreshDelay->setSingleShot(true);
    m_deviceRefreshDelay->setInterval(100);

    QVBoxLayout *centralLayout = new QVBoxLayout;

    centralLayout->addWidget(m_tipsLabel);
    centralLayout->addLayout(m_gridLayout);
    centralLayout->setSpacing(1);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(centralLayout);

    connect(m_deviceRefreshDelay, &QTimer::timeout, this, &NavWidget::onDevicesChanged);
    connect(m_bluetoothInter, &BluetoothInter::StateChanged, m_deviceRefreshDelay, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(m_wacomInter, &WacomInter::ExistChanged, m_deviceRefreshDelay, static_cast<void (QTimer::*)()>(&QTimer::start));

    m_deviceRefreshDelay->start();
    setTipsText(QString());
}

void NavWidget::onDevicesChanged()
{
    Q_ASSERT(sender() == m_deviceRefreshDelay);

    m_moduleList = MODULES;

#ifdef DISABLE_ACCOUNT
    m_moduleList.removeOne("accounts");
#endif

#ifdef DISABLE_DISPALY
    m_moduleList.removeOne("display");
#endif

#ifdef DISABLE_DEFAULT_APPLICATIONS
    m_moduleList.removeOne("defapp");
#endif

#ifdef DISABLE_PERSONALIZATION
    m_moduleList.removeOne("personalization");
#endif

#ifdef DISABLE_NETWORK
    m_moduleList.removeOne("network");
#endif

#ifdef DISABLE_SOUND
    m_moduleList.removeOne("sound");
#endif

#ifdef DISABLE_DATETIME
    m_moduleList.removeOne("datetime");
#endif

#ifdef DISABLE_POWER
    m_moduleList.removeOne("power");
#endif

#ifdef DISABLE_MOUSE
    m_moduleList.removeOne("mouse");
#endif

#ifdef DISABLE_KAYBOARD
    m_moduleList.removeOne("keyboard");
#endif

#ifdef DISABLE_SYS_UPDATE
    m_moduleList.removeOne("update");
#endif

#ifndef DISABLE_BLUETOOTH
    if (!m_bluetoothInter->state())
#endif
        m_moduleList.removeOne("bluetooth");

#ifndef DISABLE_WACOM
    if (!m_wacomInter->exist())
#endif
        m_moduleList.removeOne("wacom");

    QTimer::singleShot(1, this, &NavWidget::refershGridLayout);
}

void NavWidget::refershGridLayout()
{
    // clear
    while (QLayoutItem *item = m_gridLayout->takeAt(0))
    {
        item->widget()->deleteLater();
        delete item;
    }

    int index = 0;
    const int s = m_moduleList.size();
    for (int i = 0; i != s; ++i, ++index)
    {
        NavItemWidget *item = new NavItemWidget(m_moduleList[i]);

        connect(item, &NavItemWidget::itemClicked, this, &NavWidget::requestModule);
        connect(item, &NavItemWidget::itemEntered, this, &NavWidget::setTipsText);

        m_gridLayout->addWidget(item, index / 3, index % 3);
    }

    const int r = s % 3;
    const int a = r == 1 ? 2 : r == 2 ? 1 : 0;
    for (int i = 0; i != a; ++i, ++index)
    {
        NavItemWidget *item = new NavItemWidget(QString());

        connect(item, &NavItemWidget::itemEntered, this, &NavWidget::setTipsText);

        m_gridLayout->addWidget(item, index / 3, index % 3);
    }
}

void NavWidget::setTipsText(const QString &text)
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

    const int idx = text.isEmpty() ? -1 : MODULES.indexOf(text);

    if (idx == -1)
        m_tipsLabel->setText(tr("Navigation"));
    else
        m_tipsLabel->setText(QCoreApplication::translate(modules_scope[idx].toStdString().c_str(), modules_trans[idx].toStdString().c_str()));
}

void NavWidget::leaveEvent(QEvent *e)
{
    QWidget::leaveEvent(e);

    setTipsText(QString());
}
