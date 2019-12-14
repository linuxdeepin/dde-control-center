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
#include <DDialog>
#include <DDBusSender>

#include <QVBoxLayout>
#include <QTimer>
#include <QPushButton>
#include <QDebug>

using namespace dcc::widgets;
using namespace DCC_NAMESPACE;
using namespace commoninfo;
DWIDGET_USE_NAMESPACE

DeveloperModeWidget::DeveloperModeWidget(QWidget *parent)
    : QWidget(parent)
    , m_model(nullptr)
{
    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    m_devBtn = new QPushButton(tr("Request Root Access"));
    m_tips = new DTipLabel(tr("Sign in with UOS ID to get root privileges"));
    m_tips->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_tips->setWordWrap(true);

    vBoxLayout->setMargin(0);
    vBoxLayout->setSpacing(0);
    vBoxLayout->setContentsMargins(6, 0, 6, 0);
    vBoxLayout->addWidget(m_devBtn);
    vBoxLayout->addSpacing(10);
    vBoxLayout->addWidget(m_tips);
    vBoxLayout->addStretch();
    setLayout(vBoxLayout);

    connect(m_devBtn, &QPushButton::clicked, this, [this]{
        Q_ASSERT(m_model);
        if (!m_model->isLogin()) {
            Q_EMIT requestLogin();
        } else {
            qDebug() << "click btn and status :" << m_devBtn->isEnabled();
            m_devBtn->setEnabled(false);
            //防止出现弹窗时可以再次点击按钮
            QTimer::singleShot(100, this, [this]{
                Q_EMIT enableDeveloperMode(true);
            });
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
    if (m_model->developerModeState())
        return;

    if (!m_model->isLogin()) {
        m_tips->setText(tr("Sign in with UOS ID to get root privileges"));
    } else {
        m_tips->setText(tr("No root privileges. Please request root access in developer mode in Control Center."));
    }
}

//开发者模式变化时，更新界面
void DeveloperModeWidget::updateDeveloperModeState(const bool state)
{
    if (state) {
        //开发者模式不可逆,这里将控件disable
        m_devBtn->clearFocus();
        m_devBtn->setEnabled(false);
        m_devBtn->setText(tr("Root Access Allowed"));
        m_tips->setText(tr(""));
        m_tips->setVisible(false);
    } else {
        m_devBtn->setEnabled(true);
        m_devBtn->setText(tr("Request Root Access"));
        m_devBtn->setChecked(false);
    }
}
