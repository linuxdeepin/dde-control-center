// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "loadingitem.h"

#include <QVBoxLayout>

#include "widgets/labels/normallabel.h"

using namespace dcc::widgets;

namespace dcc {
namespace update {

LoadingItem::LoadingItem(QFrame *parent)
    : SettingsItem(parent)
    , m_messageLabel(new NormalLabel)
    , m_progress(new QProgressBar(this))
{
    m_progress->setAccessibleName("LoadingItem_progress");
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(10);

    m_progress->setRange(0, 100);
    m_progress->setFixedWidth(200);
    m_progress->setFixedHeight(7);
    m_progress->setTextVisible(false);

    layout->addStretch();
    layout->addWidget(m_progress, 0, Qt::AlignHCenter);
    layout->addWidget(m_messageLabel, 0, Qt::AlignHCenter);
    layout->addStretch();

    setLayout(layout);
    setFixedHeight(100);
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

} // namespace widgets
} // namespace dcc
