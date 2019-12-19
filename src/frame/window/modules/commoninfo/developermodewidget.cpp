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

using namespace dcc::widgets;
using namespace DCC_NAMESPACE;
using namespace commoninfo;
DWIDGET_USE_NAMESPACE

DeveloperModeWidget::DeveloperModeWidget(QWidget *parent)
    : QWidget(parent)
    , m_model(nullptr)
{
    m_inter = new QDBusInterface("com.deepin.sync.Helper",
                                                 "/com/deepin/sync/Helper",
                                                 "com.deepin.sync.Helper",
                                                 QDBusConnection::systemBus(), this);
    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    m_devBtn = new QPushButton(tr("Request Root Access"));
    auto dtip = new DTipLabel(tr("Developer mode enables using root privilege, installing and running unsigned Apps, but you may also damage the integration of your system, please use it carefully."));
    dtip->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    dtip->setWordWrap(true);

    auto utip = new DTipLabel(tr("Developer mode needs UOS ID login."));
    utip->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    utip->setWordWrap(true);

    m_offlineBtn = new QPushButton(tr("Request Root Access") + QString("(%1)").arg(tr("离线")));
    connect(m_offlineBtn, &QPushButton::clicked, this, [this]{
        QProcess *pro = new QProcess;
        pro->start("pkexec", {"/usr/lib/dde-control-center/develop-tool"});
        this->connect(pro, static_cast<void (QProcess:: *)(int)>(&QProcess::finished), this, [this](int code){
            if (0 != code)
                return;

            updateDeveloperModeState(true);
        });
    });

    vBoxLayout->setMargin(0);
    vBoxLayout->setSpacing(10);
    vBoxLayout->setContentsMargins(6, 0, 6, 0);
    vBoxLayout->addWidget(m_devBtn);
    vBoxLayout->addWidget(dtip);
    vBoxLayout->addWidget(utip);
    vBoxLayout->addStretch();
    vBoxLayout->addWidget(m_offlineBtn, 0, Qt::AlignBottom);
    setLayout(vBoxLayout);

    connect(m_devBtn, &QPushButton::clicked, this, [this]{
        Q_ASSERT(m_model);
        auto requestDev = [this]{
            qDebug() << "click btn and status :" << m_devBtn->isEnabled();
            m_devBtn->clearFocus();
            m_devBtn->setEnabled(false);
            //防止出现弹窗时可以再次点击按钮
            QTimer::singleShot(100, this, [this]{
                Q_EMIT enableDeveloperMode(true);
            });
        };

        if (!m_model->isLogin()) {
            m_enterDev = true;
            Q_EMIT requestLogin();
            connect(m_model, &CommonInfoModel::isLoginChenged, this, [requestDev, this](bool log){
                if (!log || !m_enterDev)
                    return;

                requestDev();
                m_enterDev = false;
            });
        } else {
            requestDev();
        }
    });
}

void DeveloperModeWidget::setModel(CommonInfoModel *model)
{
    m_model = model;
    onLoginChanged();
    updateDeveloperModeState(model->developerModeState());
    connect(model, &CommonInfoModel::developerModeStateChanged, this, [this](const bool state){
        //更新界面
        updateDeveloperModeState(state);

        if (!state)
            return;

        //弹窗提示重启
        DDialog dlg("", tr("To make it effective, a restart is required. Restart now?"));
        dlg.addButtons({tr("Cancel"), tr("Restart Now")});
        connect(&dlg, &DDialog::buttonClicked, this, [](int idx, QString str){
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

        m_offlineBtn->clearFocus();
        m_offlineBtn->setEnabled(false);
        m_offlineBtn->setText(tr("Root Access Allowed") + QString("%1").arg(tr("offline")));
    } else {
        m_devBtn->setEnabled(true);
        m_devBtn->setText(tr("Request Root Access"));
        m_devBtn->setChecked(false);
    }
}
