/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
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

#include "addfingedialog.h"

#include <DTitlebar>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCloseEvent>
#include <QTimer>

using namespace dcc::accounts;
using namespace DCC_NAMESPACE::accounts;
#define test false

AddFingeDialog::AddFingeDialog(const QString &thumb, DAbstractDialog *parent)
    : DAbstractDialog(parent)
    , m_timer(new QTimer(parent))
    , m_mainLayout(new QVBoxLayout(this))
    , m_titleHLayout(new QHBoxLayout)
    , m_btnHLayout(new QHBoxLayout)
    , m_fingeWidget(new FingerWidget)
    , m_thumb(thumb)
    , m_cancelBtn(new QPushButton)
    , m_addBtn(new DSuggestButton)
{
    initWidget();
    initData();
}

AddFingeDialog::~AddFingeDialog()
{
}

void AddFingeDialog::initWidget()
{
    setMinimumSize(QSize(328,391));
    m_mainLayout->setAlignment(Qt::AlignHCenter);

    DTitlebar *titleIcon = new DTitlebar();
    titleIcon->setFrameStyle(QFrame::NoFrame);//无边框
    titleIcon->setBackgroundTransparent(true);//透明
    titleIcon->setMenuVisible(false);
    titleIcon->setTitle("");
    m_titleHLayout->addWidget(titleIcon, Qt::AlignTop | Qt::AlignRight);
    m_mainLayout->addLayout(m_titleHLayout);
    m_mainLayout->addWidget(m_fingeWidget, 1);

    m_btnHLayout->addWidget(m_cancelBtn);
    m_btnHLayout->addWidget(m_addBtn);
    m_btnHLayout->setContentsMargins(10, 0, 10, 10);
    m_mainLayout->addLayout(m_btnHLayout);

    m_mainLayout->setMargin(0);
    setLayout(m_mainLayout);

    this->activateWindow();
    this->setFocus();
}

void AddFingeDialog::initData()
{
    m_cancelBtn->setText((tr("Cancel")));
    m_addBtn->setText((tr("Add Fingerprint")));
    m_addBtn->setEnabled(false);
    connect(m_cancelBtn, &QPushButton::clicked, this, &AddFingeDialog::close);
    connect(m_addBtn, &DSuggestButton::clicked, this, [=] {
        auto text = m_addBtn->text();
        if (text == tr("Done") || text == tr("Add Fingerprint")) {
            this->close();
        } else if (text == tr("Scan Again")) {
            setInitStatus();
            Q_EMIT requestEnrollThumb();
        }
    });
}

void AddFingeDialog::setFingerModel(FingerModel *model)
{
    m_model = model;
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &AddFingeDialog::enrollOverTime);
    connect(m_model, &FingerModel::enrollCompleted, this, &AddFingeDialog::enrollCompleted);
    connect(m_model, &FingerModel::enrollStagePass, this, &AddFingeDialog::enrollStagePass);
    connect(m_model, &FingerModel::enrollFailed, this, &AddFingeDialog::enrollFailed);
    connect(m_model, &FingerModel::enrollDisconnected, this, &AddFingeDialog::enrollDisconnected);
    connect(m_model, &FingerModel::enrollRetry, this, &AddFingeDialog::enrollRetry);
    connect(m_model, &FingerModel::lockedChanged, this, [this](bool locked) {
        if (locked) {
            close();
        }
    });
    m_timer->start(1000 * 60);//1min
}

void AddFingeDialog::setUsername(const QString &name)
{
    m_username = name;
}

void AddFingeDialog::enrollCompleted()
{
    if (!m_isEnrolling) {
        return;
    }

    m_isEnrolling = false;
    m_fingeWidget->finished();
    m_addBtn->setText(tr("Done"));
    m_addBtn->setEnabled(true);
    m_cancelBtn->hide();
    m_timer->stop();
    Q_EMIT requestStopEnroll(m_username);
}

void AddFingeDialog::enrollStagePass(int pro)
{
    if (!m_isEnrolling) {
        return;
    }

    m_addBtn->setEnabled(false);
    m_fingeWidget->setProsses(pro);
    m_timer->start(1000 * 60);//1min
}

void AddFingeDialog::enrollFailed(QString msg)
{
    if (!m_isEnrolling) {
        return;
    }
    m_isEnrolling = false;
    m_fingeWidget->setStatueMsg(tr("Scan Suspended"), msg, true);
    m_addBtn->setText(tr("Scan Again"));
    m_addBtn->setEnabled(true);
    m_timer->stop();

    Q_EMIT requestStopEnroll(m_username);
}
void AddFingeDialog::enrollDisconnected()
{
    Q_EMIT requestStopEnroll(m_username);

    m_isEnrolling = false;
    m_fingeWidget->setStatueMsg(tr("Scan Suspended"), tr("Scan Suspended"), true);
    m_addBtn->setText(tr("Scan Again"));
    m_addBtn->setEnabled(true);
    m_timer->stop();

    //会出现末知情况，需要与后端确认中断时是否可以停止
    Q_EMIT requestStopEnroll(m_username);
}

void AddFingeDialog::enrollOverTime()
{
    Q_EMIT requestStopEnroll(m_username);

    m_isEnrolling = false;
    m_fingeWidget->setStatueMsg(tr("Scan Suspended"), tr("Scan Overtime"), true);
    m_addBtn->setText(tr("Scan Again"));
    m_addBtn->setEnabled(true);
    m_timer->stop();

    //会出现末知情况，需要与后端确认中断时是否可以停止
    Q_EMIT requestStopEnroll(m_username);
}

void AddFingeDialog::enrollRetry(QString msg)
{
    if (!m_isEnrolling) {
        return;
    }

    m_addBtn->setEnabled(false);
    m_timer->start(1000 * 60);//1min
    m_fingeWidget->setStatueMsg(tr("Cannot recognize your fingerprint"), msg, false);
}

void AddFingeDialog::setInitStatus()
{
    m_isEnrolling = true;
    m_addBtn->setEnabled(false);
    m_addBtn->setText(tr("Add Fingerprint"));
    m_fingeWidget->reEnter();
}

void AddFingeDialog::closeEvent(QCloseEvent *event)
{
    if (m_isEnrolling) {
        Q_EMIT requestStopEnroll(m_username);
    }
    QDialog::closeEvent(event);
}

void AddFingeDialog::focusOutEvent(QFocusEvent *event)
{
    if (m_isEnrolling) {
        enrollDisconnected();
    }
}


