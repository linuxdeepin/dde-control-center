/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     sunkang <sunkang_cm@deepin.com>
 *
 * Maintainer: sunkang <sunkang_cm@deepin.com>
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

#include "developermodewidget.h"
#include "window/modules/commoninfo/commoninfomodel.h"

#include "widgets/switchwidget.h"
#include "widgets/labels/tipslabel.h"
#include "window/utils.h"

#include <DTipLabel>
#include <DTipLabel>
#include <DDialog>
#include <DDBusSender>

#include <QVBoxLayout>
#include <QTimer>
#include <QPushButton>
#include <QDebug>
#include <QDBusInterface>
#include <QDBusReply>
#include <QProcess>
#include <QFile>

using namespace dcc::widgets;
using namespace DCC_NAMESPACE;
using namespace commoninfo;
DWIDGET_USE_NAMESPACE

DeveloperModeWidget::DeveloperModeWidget(QWidget *parent)
    : QWidget(parent)
    , m_model(nullptr)
    , m_inter(new QDBusInterface("com.deepin.sync.Helper",
                                 "/com/deepin/sync/Helper",
                                 "com.deepin.sync.Helper",
                                 QDBusConnection::systemBus(), this))
    , m_developerDialog(new DeveloperModeDialog(this))
{
    m_devBtn = new QPushButton(tr("Request Root Access"));
    m_dtip = new DTipLabel(tr("Developer mode enables you to get root privileges, install and run unsigned apps not listed in app store, but your system integrity may also be damaged, please use it carefully."));
    m_dtip->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_dtip->setWordWrap(true);

    m_lab = new DLabel(tr("The feature is not available at present, please activate your system first"));
    m_lab->setWordWrap(true);
    m_lab->setVisible(false);

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->setMargin(0);
    vBoxLayout->setSpacing(10);
    vBoxLayout->setContentsMargins(ThirdPageContentsMargins);
    vBoxLayout->addWidget(m_devBtn);
    vBoxLayout->addWidget(m_lab);
    vBoxLayout->addWidget(m_dtip);
    vBoxLayout->addStretch();
    setLayout(vBoxLayout);

    connect(m_developerDialog, &DeveloperModeDialog::requestDeveloperMode, this, &DeveloperModeWidget::enableDeveloperMode);
    connect(this, &DeveloperModeWidget::enableDeveloperMode, m_developerDialog, &DeveloperModeDialog::close);
    connect(m_developerDialog, &DeveloperModeDialog::requestLogin, this, &DeveloperModeWidget::requestLogin);
    connect(m_developerDialog, &DeveloperModeDialog::requestCommit, [ this ](QString filePathName) {
        //读取机器信息证书
        QFile fFile(filePathName);
        if (!fFile.open(QIODevice::ReadOnly)) {
            qDebug() << "Can't open file for writing";
        }

        QByteArray data = fFile.readAll();
        QDBusMessage msg =  m_inter->call("EnableDeveloperMode", data);

        //当返回信息为错误接口信息才处理
        if (msg.type() == QDBusMessage::MessageType::ErrorMessage) {
            //系统通知弹窗qdbus 接口
            QDBusInterface  tInterNotify("com.deepin.dde.Notification",
                                         "/com/deepin/dde/Notification",
                                         "com.deepin.dde.Notification",
                                         QDBusConnection::sessionBus());

            //初始化Notify 七个参数
            QString in0("dde-control-center");
            uint in1 = 101;
            QString in2("preferences-system");
            QString in3("");
            QString in4("");
            QStringList in5;
            QVariantMap in6;
            int in7 = 5000;

            //截取error接口 1001:未导入证书 1002:未登录 1003:无法获取硬件信息 1004:网络异常 1005:证书加载失败 1006:签名验证失败 1007:文件保存失败
            QString msgcode = msg.errorMessage();
            msgcode = msgcode.split(":").at(0);
            if (msgcode == "1001") {
                in3 = tr("Failed to get root access");
            } else if (msgcode == "1002") {
                in3 = tr("Please sign in to your Union ID first");
            } else if (msgcode == "1003") {
                in3 = tr("Cannot read your PC information");
            } else if (msgcode == "1004") {
                in3 = tr("No network connection");
            } else if (msgcode == "1005") {
                in3 = tr("Certificate loading failed, unable to get root access");
            } else if (msgcode == "1006") {
                in3 = tr("Signature verification failed, unable to get root access");
            } else if (msgcode == "1007") {
                in3 = tr("Failed to get root access");
            }

            //系统通知认证失败 无法进入开发模式
            tInterNotify.call("Notify", in0, in1, in2, in3, in4, in5, in6, in7);
        }
    });

    //绑定选择激活开发模式窗口
    connect(m_devBtn, &QPushButton::clicked, [ this ] {
        m_developerDialog->show();
    });
}

DeveloperModeWidget::~DeveloperModeWidget()
{
    if (m_developerDialog) {
        m_developerDialog->shutdown();  // 若存在可显示对话框，则需强制关闭
        m_developerDialog->deleteLater();
    }
}

void DeveloperModeWidget::setModel(CommonInfoModel *model)
{
    m_model = model;
    m_developerDialog->setModel(m_model);
    onLoginChanged();
    if (!model->developerModeState()) {
        m_devBtn->setEnabled(model->isActivate());
        m_lab->setVisible(!model->isActivate());
        m_dtip->setVisible(model->isActivate());
    }
    updateDeveloperModeState(model->developerModeState());
    connect(model, &CommonInfoModel::developerModeStateChanged, this, [ this ](const bool state) {
        //更新界面
        updateDeveloperModeState(state);

        if (!state)
            return;

        //弹窗提示重启
        DDialog dlg("", tr("To make some features effective, a restart is required. Restart now?"), this);
        dlg.addButtons({tr("Cancel"), tr("Restart Now")});
        connect(&dlg, &DDialog::buttonClicked, this, [ = ](int idx, QString str) {
            Q_UNUSED(str);
            if (idx == 1) {
                DDBusSender()
                .service("com.deepin.SessionManager")
                .interface("com.deepin.SessionManager")
                .path("/com/deepin/SessionManager")
                .method("RequestReboot")
                .call();
            }
        });
        dlg.exec();
    });
    connect(model, &CommonInfoModel::isLoginChenged, this, &DeveloperModeWidget::onLoginChanged);
    if (!model->developerModeState()) {
        connect(model, &CommonInfoModel::LicenseStateChanged, this, [ = ](const bool & value) {
            m_devBtn->setEnabled(value);
            m_lab->setVisible(!value);
            m_dtip->setVisible(value);
        });
    }
}

void DeveloperModeWidget::onLoginChanged()
{
//    if (m_model->developerModeState())
//        return;
}

//开发者模式变化时，更新界面
void DeveloperModeWidget::updateDeveloperModeState(const bool state)
{
    QDBusReply<bool> reply = m_inter->call("IsDeveloperMode");
    if (state || reply.value()) {
        //开发者模式不可逆,这里将控件disable
        m_devBtn->clearFocus();
        m_devBtn->setEnabled(false);
        m_devBtn->setText(tr("Root Access Allowed"));

//        m_offlineBtn->clearFocus();
//        m_offlineBtn->setEnabled(false);
//        m_offlineBtn->setText(tr("Root Access Allowed"));
    } else {
        m_devBtn->setEnabled(m_model->isActivate());
        m_devBtn->setText(tr("Request Root Access"));
        m_devBtn->setChecked(false);
    }
}
