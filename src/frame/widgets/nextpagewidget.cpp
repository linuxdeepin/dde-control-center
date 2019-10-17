/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

}
}
