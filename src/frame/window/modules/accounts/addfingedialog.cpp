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

using namespace dcc::accounts;
using namespace DCC_NAMESPACE::accounts;
#define test true

AddFingeDialog::AddFingeDialog(const QString &thumb, DAbstractDialog *parent)
    : DAbstractDialog(parent)
    , m_mainLayout(new QVBoxLayout(this))
    , m_titleHLayout(new QHBoxLayout)
    , m_contentVLayout(new QVBoxLayout)
    , m_btnHLayout(new QHBoxLayout)
    , m_fingerHLayout(new QVBoxLayout)
    , m_fingeWidget(new FingerWidget)
    , m_scanBtn(new QPushButton)
    , m_doneBtn(new DSuggestButton)
    , m_thumb(thumb)
    , m_cancelBtn(new QPushButton)
    , m_addBtn(new DSuggestButton)
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

    m_fingerHLayout->addWidget(m_fingeWidget);
    m_contentVLayout->addLayout(m_fingerHLayout);

    m_btnHLayout->addWidget(m_cancelBtn);
    m_btnHLayout->addWidget(m_addBtn);
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

//    m_scanBtn->setText(tr("Scan again"));
//    m_doneBtn->setText(tr("Done"));

//    m_scanBtn->setVisible(false);
//    m_doneBtn->setVisible(false);

    m_cancelBtn->setText((tr("取消")));
    m_addBtn->setText((tr("添加")));
//    connect(m_scanBtn, &QPushButton::clicked, this, &AddFingeDialog::reEnrollStart);
//    connect(m_doneBtn, &QPushButton::clicked, this, &AddFingeDialog::saveThumb);
//    connect(m_fingeWidget, &FingerWidget::playEnd, this, &AddFingeDialog::onViewPlayEnd);
    connect(m_cancelBtn, &QPushButton::clicked, this, &AddFingeDialog::close);
}

void AddFingeDialog::setFingerModel(FingerModel *model)
{
    m_model = model;
    connect(model, &FingerModel::enrollStatusChanged, this, &AddFingeDialog::onEnrollStatusChanged);
    onEnrollStatusChanged(model->enrollStatus());
}

void AddFingeDialog::setUsername(const QString &name)
{
    m_username = name;
}

void AddFingeDialog::reEnrollStart()
{
    m_scanBtn->setVisible(false);
    m_doneBtn->setVisible(false);

    Q_EMIT requestReEnrollStart(m_thumb);

    m_fingeWidget->reEnter();
}

void AddFingeDialog::saveThumb()
{
    Q_EMIT requestSaveThumb(m_username);
    this->close();
}

void AddFingeDialog::onEnrollStatusChanged(FingerModel::EnrollStatus status)
{
    switch (status) {
    case FingerModel::EnrollStatus::Ready:
        onViewPlayEnd();
        break;
    case FingerModel::EnrollStatus::Next:
        m_fingeWidget->next();
        m_fingeWidget->setFrequency(tr("Identifying fingerprint"));
        break;
    case FingerModel::EnrollStatus::Retry:
        m_fingeWidget->setFrequency(tr("Place your finger on the fingerprint reader, or swipe upwards or downwards, and then lift it off"));
        break;
    case FingerModel::EnrollStatus::Finished:
        m_fingeWidget->finished();
        m_fingeWidget->setFrequency(tr("Fingerprint added"));
        m_scanBtn->setVisible(true);
        m_doneBtn->setVisible(true);
        break;
    }
}

void AddFingeDialog::onViewPlayEnd()
{
//    m_fingeWidget->setFrequency(tr("Place your finger on the fingerprint reader, or swipe upwards or downwards, and then lift it off"));
    m_fingeWidget->setFrequency(tr("请以手指按压指纹收集器，然后根据提示抬起"));
}

void AddFingeDialog::closeEvent(QCloseEvent *event)
{
    Q_EMIT requestStopEnroll();
    event->accept();
}
