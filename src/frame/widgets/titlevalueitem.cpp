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

#include "widgets/titlevalueitem.h"
#include "widgets/labels/tipslabel.h"

#include <DApplicationHelper>

#include <QHBoxLayout>
#include <QEvent>
#include <QPushButton>

DWIDGET_USE_NAMESPACE

namespace dcc
{

namespace widgets
{

ResizeEventFilter::ResizeEventFilter(QObject *parent) : QObject(parent) {}

bool ResizeEventFilter::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::Resize) {
        QLabel* l = qobject_cast<QLabel*>(watched);
        if (l) {
            //as we are not interested in the y offset of the rendered text, height of the rectangle doesn't matter.
            QRect r = l->fontMetrics().boundingRect(QRect(0, 0, l->width(), 100), Qt::TextWordWrap, l->text());
            l->setMinimumHeight(r.height());
            return true;
        }
    }
    return QObject::eventFilter(watched, event);
}

TitleValueItem::TitleValueItem(QFrame *parent)
    : SettingsItem(parent),
      m_title(new TipsLabel),
      m_value(new DTipLabel(""))
{
    QHBoxLayout* layout = new QHBoxLayout;

    m_value->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_value->setWordWrap(true);

    layout->setContentsMargins(10, 10, 10, 10);

    layout->addWidget(m_title);
    layout->addWidget(m_value);

    m_value->installEventFilter(new ResizeEventFilter(this));

    setLayout(layout);
}

void TitleValueItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

void TitleValueItem::setValue(const QString &value)
{
    m_value->setText(value);
}

void TitleValueItem::setWordWrap(const bool enable)
{
    m_value->setWordWrap(enable);
}


TitleAuthorizedItem::TitleAuthorizedItem(QFrame *parent)
    : SettingsItem(parent)
    , m_title(new TipsLabel)
    , m_value(new DTipLabel(""))
    , m_pActivatorBtn(new QPushButton)
{
    QHBoxLayout* layout = new QHBoxLayout;

    m_value->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_value->setWordWrap(true);

    layout->setContentsMargins(10, 10, 10, 10);

    layout->addWidget(m_title);
    layout->addWidget(m_value);
    layout->addWidget(m_pActivatorBtn);
    m_pActivatorBtn->setFocusPolicy(Qt::NoFocus);

    m_value->installEventFilter(new ResizeEventFilter(this));

    setLayout(layout);

    //传递button的点击信号
    connect(m_pActivatorBtn, SIGNAL(clicked()), this, SIGNAL(clicked()));
}

void TitleAuthorizedItem::setTitle(const QString& title)
{
    m_title->setText(title);
}

void TitleAuthorizedItem::setValue(const QString& value)
{
    m_value->setText(value);
}

void TitleAuthorizedItem::setWordWrap(bool enable)
{
    m_value->setWordWrap(enable);
}

void TitleAuthorizedItem::setButtonText(const QString &str)
{
    m_pActivatorBtn->setText(str);
}

void TitleAuthorizedItem::setVisable(bool value)
{
    m_pActivatorBtn->setVisible(value);
}

void TitleAuthorizedItem::setValueForegroundRole(const QColor &color)
{
    auto pa = DApplicationHelper::instance()->palette(m_value);
    pa.setBrush(DPalette::TextTips, color);
    DApplicationHelper::instance()->setPalette(m_value, pa);
}

}

}
