/*
 * Copyright (C) 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <andywang_cm@deepin.com>
 *
 * Maintainer: andywang <andywang_cm@deepin.com>
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

#include "detailpage.h"
#include "modules/bluetooth/device.h"
#include "modules/bluetooth/adapter.h"
#include "modules/bluetooth/bluetoothmodel.h"
#include "widgets/translucentframe.h"
#include "widgets/titlelabel.h"
#include "window/gsettingwatcher.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QString>
#include <QDBusInterface>
#include <QFileDialog>
#include <QApplication>
#include <QDir>
#include <QGSettings>

#include <DDesktopServices>

using namespace dcc;
using namespace dcc::bluetooth;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::bluetooth;

DetailPage::DetailPage(const BluetoothModel *model, const Adapter *adapter, const Device *device)
    : ContentWidget()
    , m_bluetoothModel(model)
    , m_adapter(adapter)
    , m_device(device)
    , m_transFile(new QFileDialog(this))
    , m_setting(new QGSettings("com.deepin.dde.control-center", QByteArray(), this))
{
    setAccessibleName("DetailPage");
    m_ignoreButton = new QPushButton(tr("Ignore this device"));
    m_ignoreButton->setObjectName("ignoreButton");

    m_disconnectButton = new QPushButton(tr("Disconnect"));
    m_disconnectButton->setObjectName("disconnectButton");

    m_connectButton = new QPushButton(tr("Connect"));
    m_connectButton->setObjectName("connectButton");

    m_transfileButton = new QPushButton(tr("Send Files"));
    m_transfileButton->setObjectName("transfileButton");
    m_transfileButton->setEnabled(m_bluetoothModel->canTransportable());
    setTitle(device->name());

    m_transFile->setModal(true);
    m_transFile->setFileMode(QFileDialog::ExistingFiles);
    m_transFile->setAcceptMode(QFileDialog::AcceptOpen);
    m_transFile->setDirectory(QDir::homePath());
    connect(m_transFile, &QFileDialog::finished, this, [ = ] (int result) {
       if (result == QFileDialog::Accepted) {
           QStringList selectedFiles = m_transFile->selectedFiles();
           if (selectedFiles.count() <= 0)
               return;

           QDBusInterface inter("com.deepin.filemanager.filedialog"
                                    , "/com/deepin/filemanager/filedialogmanager"
                                    , "com.deepin.filemanager.filedialogmanager"
                                    , QDBusConnection::sessionBus());
           if (!inter.isValid()) {
               qDebug() << "send file interface is not valid: " << QDBusConnection::sessionBus().lastError().message();
               return;
           }

           // 蓝牙传输dbus接口: SendFile(destination string, filename string) 接口支持文件多选
           inter.call("showBluetoothTransDialog", m_device->address(), selectedFiles);
       }
    });

    dcc::widgets::TranslucentFrame *frame = new dcc::widgets::TranslucentFrame;
    QVBoxLayout *layout = new QVBoxLayout(frame);
    layout->setSpacing(0);
    layout->setContentsMargins(8, 8, 8, 8);

    DIconButton *backWidgetBtn = new DIconButton(this);
    backWidgetBtn->setAccessibleName("back_btn");
    backWidgetBtn->setFlat(true);
    backWidgetBtn->setFixedSize(30, 48);
    backWidgetBtn->setIcon(QStyle::SP_ArrowBack);
    DStyle::setFocusRectVisible(backWidgetBtn, false);
    layout->addWidget(backWidgetBtn, Qt::AlignLeft);

    m_devNameLabel = new TitleLabel(device->name());
    layout->addWidget(m_devNameLabel, 0, Qt::AlignCenter);
    layout->setSpacing(10);

    m_editDevAlias = new QLineEdit;
    m_editDevAlias->setPlaceholderText(device->alias().isEmpty() ? device->name() : device->alias());

    layout->addWidget(m_editDevAlias);
    layout->addWidget(m_disconnectButton);
    layout->addWidget(m_connectButton);
    layout->addWidget(m_ignoreButton);
    layout->addWidget(m_transfileButton);
    layout->addStretch();
    setContent(frame);

    onDeviceStatusChanged();

    connect(m_ignoreButton, &QPushButton::clicked, this, [this] {
        Q_EMIT requestIgnoreDevice(m_adapter, m_device);
        if (QApplication::focusWidget()) {
            QApplication::focusWidget()->clearFocus();
        }
    });
    connect(m_disconnectButton, &QPushButton::clicked, this, [this] {
        Q_EMIT requestDisconnectDevice(m_device);
        if (QApplication::focusWidget()) {
            QApplication::focusWidget()->clearFocus();
        }
    });
    connect(m_connectButton, &QPushButton::clicked, this, [this] {
        Q_EMIT requestConnectDevice(m_device, m_adapter);
        if (QApplication::focusWidget()) {
            QApplication::focusWidget()->clearFocus();
        }
    });
    connect(m_transfileButton, &QPushButton::clicked, this, [this] {
        // 调用接口选择文件进行传输
        m_transFile->show();
    });
    connect(m_editDevAlias, &QLineEdit::textChanged, this, [ = ](const QString &str){
        if (str.length() > 32) {
            m_editDevAlias->backspace();
            DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
        }
    });
    connect(m_bluetoothModel, &BluetoothModel::transportableChanged, m_transfileButton, &QPushButton::setEnabled);

    connect(m_device, &Device::nameChanged, m_devNameLabel, &QLabel::setText);
    connect(m_device, &Device::aliasChanged, m_editDevAlias, &QLineEdit::setText);
    connect(m_device, &Device::stateChanged, this, [this] {
        onDeviceStatusChanged();
    });
    connect(m_device, &Device::pairedChanged, this, [this](const bool paired) {
        if (!paired) {
            Q_EMIT back();
        }
    });
    //收到蓝牙关闭引发的closeDetailPage信号后，直接关闭三级页面不需要等待后端服务关闭成功的反馈信号
    connect(adapter, &Adapter::closeDetailPage, this, [this] {
        if (isVisible())
            Q_EMIT back();
    });
    connect(m_editDevAlias, &QLineEdit::editingFinished, this, &DetailPage::onDeviceAliasChanged);
    connect(adapter, &Adapter::destroyed, this, &DetailPage::back);
    connect(backWidgetBtn, &DIconButton::clicked, this, &DetailPage::back);
    connect(m_setting, &QGSettings::changed, this,  &DetailPage::onSettingChanged);
}

DetailPage::~DetailPage()
{
    if (m_transFile)
        m_transFile->deleteLater();
}

void DetailPage::onSettingChanged(const QString &key)
{
    if (key == "bluetoothTransfile") {

        if (!m_bluetoothModel->canSendFile() || !m_device->canSendFile()) {
            m_transfileButton->setVisible(false);
        }
        else {
            if (m_setting->get("bluetoothTransfile").toString() == "Enabled") {
                m_transfileButton->setVisible(true);
                m_transfileButton->setEnabled(true);
            }
            else if (m_setting->get("bluetoothTransfile").toString() == "Disabled") {
                m_transfileButton->setVisible(true);
                m_transfileButton->setEnabled(false);
            }
            else {
                m_transfileButton->setVisible(false);
            }
        }

    }
}

void DetailPage::onDeviceStatusChanged()
{
    switch (m_device->state()) {
    case Device::StateConnected:
        if (m_device->connectState()) {
            m_disconnectButton->show();
            m_connectButton->hide();
            onSettingChanged("bluetoothTransfile");
            m_ignoreButton->setEnabled(true);
        } else {
            m_connectButton->show();
            m_connectButton->setText(tr("Connect"));
            m_connectButton->setEnabled(true);
            m_disconnectButton->hide();
            m_transfileButton->hide();
            m_ignoreButton->setEnabled(true);
        }
        break;
    case Device::StateAvailable:
        m_connectButton->show();
        m_connectButton->setText(tr("Connecting"));
        m_connectButton->setDisabled(true);
        m_disconnectButton->hide();
        m_transfileButton->hide();
        m_ignoreButton->setEnabled(false);
        break;
    case Device::StateDisconnecting:
        m_connectButton->show();
        m_connectButton->setText(tr("Disconnecting"));
        m_connectButton->setDisabled(true);
        m_disconnectButton->hide();
        m_transfileButton->hide();
        m_ignoreButton->setEnabled(false);

        break;
    case Device::StateUnavailable:
        m_connectButton->show();
        m_connectButton->setText(tr("Connect"));
        m_connectButton->setEnabled(true);
        m_disconnectButton->hide();
        m_transfileButton->hide();
        m_ignoreButton->setEnabled(true);
        break;
    }
}

void DetailPage::removeDevice(const QString &id)
{
    if(id == m_device->id())
        Q_EMIT requestBack();
}

void DetailPage::onDeviceAliasChanged()
{
    QString devAlias = m_editDevAlias->text();
    QString devName(m_device->name());
    if (devAlias.isEmpty()) {
        m_editDevAlias->setPlaceholderText(m_device->name());
        Q_EMIT requestSetDevAlias(m_device, devName);
    } else if (devAlias != m_device->alias()) {
        Q_EMIT requestSetDevAlias(m_device, devAlias);
    }
}
