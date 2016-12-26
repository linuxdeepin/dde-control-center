/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "loadingitem.h"

#include <QVBoxLayout>
#include <QTimer>

#include "loadingindicator.h"
#include "labels/normallabel.h"

using namespace dcc::widgets;

namespace dcc {
namespace update {

LoadingItem::LoadingItem(QFrame *parent)
    : SettingsItem(parent),
      m_messageLabel(new NormalLabel),
      m_indicator(new LoadingIndicator)
{
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addStretch();
    layout->addWidget(m_indicator, 0, Qt::AlignHCenter);
    layout->addWidget(m_messageLabel, 0, Qt::AlignHCenter);
    layout->addStretch();

    setLayout(layout);
    setFixedHeight(100);

    QTimer::singleShot(0, m_indicator, &LoadingIndicator::play);
}

void LoadingItem::setIndicatorVisible(bool visible)
{
    m_indicator->setVisible(visible);
}

void LoadingItem::setMessage(const QString &message)
{
    m_messageLabel->setText(message);
}

} // namespace widgets
} // namespace dcc
