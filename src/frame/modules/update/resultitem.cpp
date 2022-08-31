// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "resultitem.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <dhidpihelper.h>

#include "widgets/labels/normallabel.h"
#include "widgets/basiclistdelegate.h"

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace update {

ResultItem::ResultItem(QFrame *parent)
    : SettingsItem(parent),
      m_message(new dcc::widgets::NormalLabel),
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
        m_pix = ":/update/themes/common/icons/noactive.svg";
        m_icon->setPixmap(DHiDPIHelper::loadNxPixmap(m_pix));
        setMessage(tr("Your system is not authorized, please activate first"));
        break;
    case ShowStatus::IsSuccessed:
        m_pix = ":/update/themes/common/icons/success.svg";
        m_icon->setPixmap(DHiDPIHelper::loadNxPixmap(m_pix));
        setMessage(tr("Update successful"));
        break;
    case ShowStatus::IsFailed:
        m_pix = ":/update/themes/common/icons/failed.svg";
        m_icon->setPixmap(DHiDPIHelper::loadNxPixmap(m_pix));
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

} // namespace update
} // namespace dcc
