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
    , m_mainLayout(new QVBoxLayout(this))
    , m_titleHLayout(new QHBoxLayout)
    , m_contentVLayout(new QVBoxLayout)
    , m_btnHLayout(new QHBoxLayout)
    , m_fingerHLayout(new QVBoxLayout)
    , m_fingeWidget(new FingerWidget)
    , m_scanBtn(new DSuggestButton)
    , m_doneBtn(new DSuggestButton)
    , m_thumb(thumb)
    , m_cancelBtn(new QPushButton)
    , m_addBtn(new DSuggestButton)
    , m_titleTip(new DTipLabel)
    , m_qtimerTitleTip(new QTimer())
    , m_qtimerMsgTip(new QTimer())
{
    initWidget();
    initData();
}

void AddFingeDialog::initWidget()
{
    setMinimumSize(QSize(328,391));
    DTitlebar *titleIcon = new DTitlebar();
    titleIcon->setFrameStyle(QFrame::NoFrame);//无边框
    titleIcon->setBackgroundTransparent(true);//透明
    titleIcon->setMenuVisible(false);
    titleIcon->setTitle("");
    m_titleHLayout->addWidget(titleIcon, Qt::AlignTop);
    m_mainLayout->addLayout(m_titleHLayout);
    QFont font;
    font.setBold(true);
    m_titleTip->setFont(font);
    m_titleTip->setText(tr("放置手指"));
    m_fingerHLayout->addWidget(m_titleTip);
    m_fingerHLayout->addWidget(m_fingeWidget);
    m_contentVLayout->addLayout(m_fingerHLayout);

    m_btnHLayout->addWidget(m_cancelBtn);
    m_btnHLayout->addWidget(m_addBtn);
    m_btnHLayout->addWidget(m_scanBtn);
    m_btnHLayout->addWidget(m_doneBtn);
    m_btnHLayout->setContentsMargins(10,0,10,10);
    m_contentVLayout->addLayout(m_btnHLayout);
    m_mainLayout->addLayout(m_contentVLayout);

    m_mainLayout->setMargin(0);
    setLayout(m_mainLayout);
    m_cancelBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_addBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void AddFingeDialog::initData()
{
//    setWindowTitle(tr("Add Fingerprint"));

    m_scanBtn->setText(tr("Scan again"));
    m_doneBtn->setText(tr("Done"));

    m_scanBtn->setVisible(false);
    m_doneBtn->setVisible(false);

    m_cancelBtn->setText((tr("取消")));
    m_addBtn->setText((tr("添加")));
    connect(m_cancelBtn, &QPushButton::clicked, this, &AddFingeDialog::close);
    connect(m_scanBtn, &QPushButton::clicked, this, &AddFingeDialog::requestReEnrollThumb);//重新录入后，界面变化还需要再确认
    connect(m_scanBtn, &DSuggestButton::clicked, this, &AddFingeDialog::setInitStatus);//录入中断，或断开超时
    connect(m_addBtn, &QPushButton::clicked, this, &AddFingeDialog::requestEnrollThumb);
    connect(m_scanBtn, &QPushButton::clicked, this, &AddFingeDialog::close);
    connect(m_doneBtn, &QPushButton::clicked, this, &AddFingeDialog::close);
}

void AddFingeDialog::setFingerModel(FingerModel *model)
{
    m_model = model;
    connect(m_model, &FingerModel::enrollCompleted, this, &AddFingeDialog::enrollCompleted);
    connect(m_model, &FingerModel::enrollStagePass, this, &AddFingeDialog::enrollStagePass);
    connect(m_model, &FingerModel::enrollFailed, this, &AddFingeDialog::enrollFailed);
    connect(m_model, &FingerModel::enrollDisconnected, this, &AddFingeDialog::enrollDisconnected);
    connect(m_model, &FingerModel::enrollRetry, this, &AddFingeDialog::enrollRetry);

//    connect(m_model, &FingerModel::enrollFailed, this, &AddFingeDialog::enrollFailed)
}

void AddFingeDialog::setUsername(const QString &name)
{
    m_username = name;
}

void AddFingeDialog::setDefaultMsgTip()
{
    m_fingeWidget->setFrequency(tr("请以手指按压指纹收集器，然后根据提示抬起"));
}

void AddFingeDialog::enrollCompleted()
{
    m_titleTip->setText(tr("指纹录入完成！"));
    m_fingeWidget->setFrequency("");
    m_fingeWidget->finished();
    m_doneBtn->setVisible(true);
    m_addBtn->setVisible(false);
}

void AddFingeDialog::enrollStagePass(int pro)
{
    if (pro < 35) {
        m_titleTip->setVisible(false);
        m_fingeWidget->setFrequency(tr("请抬起手指，再次按压"));
        m_fingeWidget->next();
    }
    if (pro >= 35 && pro <100) {
        m_fingeWidget->setFrequency(tr("请抬起手指，调整按压区域，继续录入边缘指纹"));
    }
}

void AddFingeDialog::enrollFailed(QString msg)
{
    m_titleTip->setText("录入中断");
    m_fingeWidget->setFrequency(msg);
    m_addBtn->setVisible(false);
    m_scanBtn->setVisible(true);
}

void AddFingeDialog::setInitStatus()
{
    m_cancelBtn->setVisible(true);
    m_addBtn->setVisible(true);
    m_scanBtn->setVisible(false);
    m_doneBtn->setVisible(false);
    m_titleTip->setText(tr("放置手指"));
    m_fingeWidget->setFrequency(tr("请以手指压指纹收集器，然后根据提示抬起"));
}

void AddFingeDialog::enrollDisconnected()
{
    m_titleTip->setText(tr("录入中断"));
    m_fingeWidget->setFrequency(tr("录入中断"));
    m_addBtn->setVisible(false);
    m_scanBtn->setVisible(true);
}

void AddFingeDialog::enrollRetry(QString msg)
{
    m_qtimerTitleTip->stop();
    m_qtimerTitleTip->setInterval(1000);
    m_titleTip->setText(tr("无法识别"));
    connect(m_qtimerTitleTip, &QTimer::timeout, this, [this] {
        setDefaultTitleTip();
    });
    m_qtimerTitleTip->start();

    m_qtimerMsgTip->setSingleShot(true);
    m_fingeWidget->setFrequency(msg);
    m_qtimerMsgTip->setInterval(2000);
    connect(m_qtimerMsgTip, &QTimer::timeout, [this] {setDefaultMsgTip();});
    m_qtimerMsgTip->start();


}

void AddFingeDialog::onViewPlayEnd()
{
    m_fingeWidget->setFrequency(tr("Place your finger on the fingerprint reader, or swipe upwards or downwards, and then lift it off"));

}

void AddFingeDialog::setDefaultTitleTip()
{
    m_titleTip->setText(tr("放置手指"));
}

void AddFingeDialog::closeEvent(QCloseEvent *event)
{
    Q_EMIT requestStopEnroll();
    event->accept();
}
