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

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCloseEvent>

using namespace dcc::accounts;
using namespace DCC_NAMESPACE::accounts;
#define test true

AddFingeDialog::AddFingeDialog(const QString &thumb, QDialog *parent)
    : QDialog(parent)
    , m_mainContentLayout(new QVBoxLayout)
    , m_cancleaddLayout(new QHBoxLayout)
    , m_fingeWidget(new FingerWidget)
    , m_scanBtn(new QPushButton)
    , m_doneBtn(new DSuggestButton)
    , m_thumb(thumb)
{
    initWidget();
    initData();
}

void AddFingeDialog::initWidget()
{
    resize(400, 400);
    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::WindowCloseButtonHint;
    flags |= Qt::WindowMaximizeButtonHint;
    setWindowFlags(flags);

    setAttribute(Qt::WA_DeleteOnClose, true);//destroy this object when this window is closed

    m_mainContentLayout->setMargin(0);
    m_mainContentLayout->setSpacing(0);

    m_cancleaddLayout->setContentsMargins(10, 0, 5, 10);
    m_cancleaddLayout->addWidget(m_scanBtn);
    m_cancleaddLayout->addSpacing(10);
    m_cancleaddLayout->addWidget(m_doneBtn);

    m_mainContentLayout->addSpacing(20);
    m_mainContentLayout->addWidget(m_fingeWidget);
    m_mainContentLayout->addSpacing(50);
    m_mainContentLayout->addLayout(m_cancleaddLayout);
    m_mainContentLayout->addSpacing(10);

    setLayout(m_mainContentLayout);
    m_scanBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_doneBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void AddFingeDialog::initData()
{
    setWindowTitle(tr("Add Fingerprint"));

    m_scanBtn->setText(tr("Scan again"));
    m_doneBtn->setText(tr("Done"));

    if(test) {
        m_scanBtn->setVisible(true);
        m_doneBtn->setVisible(true);
    } else {
        m_scanBtn->setVisible(false);
        m_doneBtn->setVisible(false);
    }

    connect(m_scanBtn, &QPushButton::clicked, this, &AddFingeDialog::reEnrollStart);
    connect(m_doneBtn, &QPushButton::clicked, this, &AddFingeDialog::saveThumb);
    connect(m_fingeWidget, &FingerWidget::playEnd, this, &AddFingeDialog::onViewPlayEnd);
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
    m_fingeWidget->setFrequency(tr("Place your finger on the fingerprint reader, or swipe upwards or downwards, and then lift it off"));
}

void AddFingeDialog::closeEvent(QCloseEvent *event)
{
    Q_EMIT requestStopEnroll();
    event->accept();
}
