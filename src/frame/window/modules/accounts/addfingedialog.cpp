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
    connect(m_addBtn, &QPushButton::clicked, this, &AddFingeDialog::requestEnrollThumb);
}

void AddFingeDialog::setFingerModel(FingerModel *model)
{
    m_model = model;
}

void AddFingeDialog::setUsername(const QString &name)
{
    m_username = name;
}

void AddFingeDialog::setDefaultMsgTip()
{
    m_fingeWidget->setFrequency(tr("请以手指按压指纹收集器，然后根据提示抬起"));
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
