// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "widgets/nextpagewidget.h"

#include <QHBoxLayout>
#include <QMouseEvent>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace widgets {

NextPageWidget::NextPageWidget(QFrame *parent, bool bLeftInterval)
    : SettingsItem(parent)
    , m_title(new NormalLabel)
    , m_value(new TipsLabel)
    , m_nextPageBtn(new dcc::widgets::NextButton)
    , m_icon(new TipsLabel)
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    if (bLeftInterval) {
        mainLayout->setContentsMargins(20, 0, 10, 0);
    } else {
        mainLayout->setContentsMargins(10, 0, 10, 0);
    }

    mainLayout->addWidget(m_title);
    mainLayout->addStretch();
    mainLayout->addSpacing(18);
    mainLayout->addWidget(m_value, 0, Qt::AlignRight | Qt::AlignCenter);
    mainLayout->addWidget(m_nextPageBtn);
    mainLayout->addWidget(m_icon);

    setFixedHeight(36);
    setLayout(mainLayout);
    setObjectName("NextPageWidget");

    connect(m_nextPageBtn, &widgets::NextButton::clicked, this, &NextPageWidget::acceptNextPage);
    connect(m_nextPageBtn, &widgets::NextButton::clicked, this, &NextPageWidget::clicked);
}

void NextPageWidget::setTitle(const QString &title)
{
    m_title->setText(title);

    setAccessibleName(title);
}

void NextPageWidget::setValue(const QString &value)
{
    m_value->setText(value);
}

void NextPageWidget::setIcon(const QPixmap &icon)
{
    m_value->setPixmap(icon);
}

void NextPageWidget::setRightTxtWordWrap(bool state)
{
    //Destroy setFixedHeight
    setMaximumHeight(100);
    m_value->setWordWrap(state);
    m_nextPageBtn->setMinimumWidth(3);
}

void NextPageWidget::mouseReleaseEvent(QMouseEvent *e)
{
    SettingsItem::mousePressEvent(e);

    if (e->button() != Qt::LeftButton) {
        return;
    }

    Q_EMIT selected();
    Q_EMIT clicked();
}

void NextPageWidget::setBtnHiden(const bool hiden)
{
    m_nextPageBtn->setHidden(hiden);
}

void NextPageWidget::setIconIcon(const QPixmap &icon)
{
    m_icon->setPixmap(icon);
}

}
}
