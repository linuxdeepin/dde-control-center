//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "resultitem.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <dhidpihelper.h>
#include <DIcon>

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

ResultItem::ResultItem(QFrame *parent)
    : SettingsItem(parent),
      m_message(new QLabel),
      m_icon(new QLabel),
      m_pix("")
{
    m_icon->setFixedSize(128, 128);

    m_message->setWordWrap(true);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addSpacing(15);
    layout->addWidget(m_icon, 0, Qt::AlignHCenter);
    layout->addSpacing(15);
    layout->addWidget(m_message, 0, Qt::AlignHCenter);
    layout->addSpacing(15);

    setLayout(layout);
}

void ResultItem::setSuccess(ShowStatus type)
{
    switch (type) {
    case ShowStatus::NoActive:
        m_pix = ":/icons/deepin/builtin/icons/noactive.svg";
        m_icon->setPixmap(DIcon::loadNxPixmap(m_pix));
        setMessage(tr("Your system is not authorized, please activate first"));
        break;
    case ShowStatus::IsSuccessed:
        m_pix = ":/icons/deepin/builtin/icons/success.svg";
        m_icon->setPixmap(DIcon::loadNxPixmap(m_pix));
        setMessage(tr("Update successful"));
        break;
    case ShowStatus::IsFailed:
        m_pix = ":/icons/deepin/builtin/icons/failed.svg";
        m_icon->setPixmap(DIcon::loadNxPixmap(m_pix));
        setMessage(tr("Failed to update"));
        break;
    default:
        qDebug() << "unknown status!!!";
        break;
    }
}

void ResultItem::setMessage(const QString &message)
{
    m_message->setText(message);
}
