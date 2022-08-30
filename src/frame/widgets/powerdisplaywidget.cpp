// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "widgets/powerdisplaywidget.h"
#include "widgets/labels/normallabel.h"

#include <QHBoxLayout>
#include <QDebug>
#include <QMouseEvent>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace widgets {
PowerDisplayWidget::PowerDisplayWidget(const QString &title, QWidget *parent)
    : PowerDisplayWidget(parent, new QLabel(title), new QLabel)
{

}

PowerDisplayWidget::PowerDisplayWidget(QWidget *parent, QLabel *leftWidget, QLabel *rightWidget)
    : SettingsItem(parent)
    , m_leftWidget(leftWidget)
    , m_rightWidget(rightWidget)
{
    if (!m_leftWidget)
        m_leftWidget = new QLabel();

    if (!m_rightWidget)
        m_rightWidget = new QLabel();

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(m_leftWidget, 0, Qt::AlignVCenter);
    mainLayout->addWidget(m_rightWidget, 0, Qt::AlignVCenter);

    m_leftWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_rightWidget->setAlignment(Qt::AlignRight);

    setLayout(mainLayout);
}

void PowerDisplayWidget::setTitle(const QString &title)
{
    m_leftWidget->setWordWrap(true);
    m_leftWidget->setText(title);
    m_leftWidget->setWordWrap(true);

    setAccessibleName(title);
}

void PowerDisplayWidget::setText(const QString &text)
{
    m_rightWidget->setWordWrap(true);
    m_rightWidget->setText(text);
    m_rightWidget->setWordWrap(true);
}
}
}
