//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "loadingitem.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QIcon>
#include <QPainter>
#include <DSysInfo>
#include <DIconTheme>

DGUI_USE_NAMESPACE
DCORE_USE_NAMESPACE

LoadingItem::LoadingItem(QFrame *parent)
    : QWidget(parent)
    , m_messageLabel(new QLabel)
    , m_progress(new QProgressBar(this))
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(10);

    m_progress->setAccessibleName("LoadingItem_progress");
    m_progress->setRange(0, 100);
    m_progress->setFixedWidth(200);
    m_progress->setFixedHeight(7);
    m_progress->setTextVisible(false);

    QVBoxLayout *imgLayout = new QVBoxLayout;
    imgLayout->setAlignment(Qt::AlignCenter);
    m_labelImage = new QLabel;
    m_labelImage->setMinimumSize(128, 128);
    imgLayout->addWidget(m_labelImage, 0, Qt::AlignTop);

    QHBoxLayout *txtLayout = new QHBoxLayout;
    txtLayout->setAlignment(Qt::AlignCenter);
    m_labelText = new QLabel;
    txtLayout->addWidget(m_labelText);

    layout->addStretch();
    layout->addLayout(imgLayout);
    layout->addLayout(txtLayout);
    layout->addWidget(m_progress, 0, Qt::AlignHCenter);
    layout->addWidget(m_messageLabel, 0, Qt::AlignHCenter);
    layout->addStretch();

    setLayout(layout);
}

void LoadingItem::setProgressValue(int value)
{
    m_progress->setValue(value);
}

void LoadingItem::setProgressBarVisible(bool visible)
{
    m_progress->setVisible(visible);
}

void LoadingItem::setMessage(const QString &message)
{
    m_messageLabel->setText(message);
}

void LoadingItem::setVersionVisible(bool state)
{
    m_labelText->setVisible(state);
}

void LoadingItem::setSystemVersion(const QString &version)
{
    Q_UNUSED(version);
    QString uVersion = DSysInfo::uosProductTypeName() + " " + DSysInfo::majorVersion();
    if (DSysInfo::uosType() != DSysInfo::UosServer)
        uVersion.append(" " + DSysInfo::uosEditionName());
    m_labelText->setText(uVersion);
}

void LoadingItem::setImageVisible(bool state)
{
    m_labelImage->setVisible(state);
}

//image or text only use one
void LoadingItem::setImageOrTextVisible(bool state)
{
    qDebug() << Q_FUNC_INFO << state;

    setVersionVisible(state);
    setImageVisible(true);

    QString path = "";
    if (state) {
        m_labelImage->setPixmap(DIconTheme::findQIcon("icon_success").pixmap({128, 128}));
    } else {
        m_labelImage->setPixmap(QIcon(":/icons/deepin/builtin/icons/dcc_checking_update.svg").pixmap({128, 128}));
    }
}

//image and text all use or not use
void LoadingItem::setImageAndTextVisible(bool state)
{
    setVersionVisible(state);
    setImageVisible(state);
}
