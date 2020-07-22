/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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
#include "datewidget.h"
#include "widgets/labels/normallabel.h"

#include <QIntValidator>
#include <QRegExp>
#include <QRegExpValidator>
#include <QHBoxLayout>
#include <QMouseEvent>

using namespace dcc::widgets;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::datetime;

DWIDGET_USE_NAMESPACE

DateWidget::DateWidget(Type type, int minimum, int maximum, QFrame *parent)
    : SettingsItem(parent)
    , m_type(type)
    , m_minimum(minimum)
    , m_maximum(maximum)
    , m_lineEdit(new TimeSetLineEdit)
    , m_label(new TimeSetEdit)
    , m_addBtn(new DIconButton(this))
    , m_reducedBtn(new DIconButton(this))
{
    setFixedHeight(36);

    m_addBtn->setIcon(DStyle::StandardPixmap::SP_IncreaseElement);
    m_reducedBtn->setIcon(DStyle::StandardPixmap::SP_DecreaseElement);

    m_lineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    m_lineEdit->setObjectName("DCC-Datetime-QLineEdit");
    //m_lineEdit->setFrame(false) , 这样设置只能去掉边框,无法去除背景,目前只能使用qss进行设置
    m_lineEdit->setStyleSheet("background:transparent; border-width:0; border-style:outset");
    m_addBtn->setObjectName("DCC-Datetime-Datewidget-Add");
    m_reducedBtn->setObjectName("DCC-Datetime-Datewidget-Reduce");

    m_label->setParent(m_lineEdit);
    m_label->move(0, 0);
    m_addBtn->setParent(m_lineEdit);
    m_reducedBtn->setParent(m_lineEdit);

    if (m_type == Year) {
        m_label->setText(tr("Year"));
        m_lineEdit->setAccessibleName("TimeYear");
    } else if (m_type == Month) {
        m_label->setText(tr("Month"));
        m_lineEdit->setAccessibleName("TimeMonth");
    } else {
        m_label->setText(tr("Day"));
        m_lineEdit->setAccessibleName("TimeDay");
    }

    m_lineEdit->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    setRange(minimum, maximum);
    m_lineEdit->installEventFilter(this);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    QHBoxLayout *lLayout = new QHBoxLayout;
    lLayout->setMargin(0);
    lLayout->setSpacing(0);
    lLayout->addWidget(m_reducedBtn);
    lLayout->addStretch();
    lLayout->addWidget(m_lineEdit);

    QHBoxLayout *rLayout = new QHBoxLayout;
    rLayout->setMargin(0);
    rLayout->setSpacing(0);
    rLayout->addWidget(m_label);
    rLayout->addStretch();
    rLayout->addWidget(m_addBtn);

    layout->addLayout(lLayout);
    layout->addSpacing(5);
    layout->addLayout(rLayout);
    setLayout(layout);

    connect(m_addBtn, &DIconButton::clicked, this, &DateWidget::slotAdd);
    connect(m_reducedBtn, &DIconButton::clicked, this, &DateWidget::slotReduced);

    connect(m_lineEdit, &QLineEdit::editingFinished, [this] {
        fixup();
        Q_EMIT editingFinished();
    });

    connect(m_label, &TimeSetEdit::notifyClicked, [this] {
        m_lineEdit->setFocus();
    });

    connect(m_lineEdit, &TimeSetLineEdit::notifyClicked, [this] {
        m_lineEdit->setFocus();
    });
}

void DateWidget::setValue(const int &value)
{
    m_lineEdit->blockSignals(true);
    m_lineEdit->setText(QString::number(value));
    m_lineEdit->blockSignals(false);
}

int DateWidget::value() const
{
    return m_lineEdit->text().toInt();
}

void DateWidget::slotAdd()
{
    int value = m_lineEdit->text().toInt() + 1;

    if (value < m_minimum) {
        value = m_maximum;
    } else if (value > m_maximum) {
        value = m_minimum;
    }

    m_lineEdit->setText(QString::number(value));

    Q_EMIT notifyClickedState(true);
}

void DateWidget::slotReduced()
{
    int value = m_lineEdit->text().toInt() - 1;

    if (value < m_minimum) {
        value = m_maximum;
    } else if (value > m_maximum) {
        value = m_minimum;
    }

    m_lineEdit->setText(QString::number(value));

    Q_EMIT notifyClickedState(false);
}

void DateWidget::fixup()
{
    int value = m_lineEdit->text().toInt();
    value = qMin(m_maximum, qMax(m_minimum, value));
    m_lineEdit->setText(QString::number(value));
}

int DateWidget::maximum() const
{
    return m_maximum;
}

void DateWidget::setRange(int minimum, int maximum)
{
    m_minimum = minimum;
    m_maximum = maximum;

    //用正则表达岁来限制输入格式,这样不会出现可以输入"000000..."的情况
    QRegExp rx;
    if (m_type == Year) {
        rx.setPattern("^(19[7-9]\\d|[2-9]\\d\\d\\d)$"); //1970-9999
    } else if (m_type == Month) {
        rx.setPattern("^([1-9][0-2]{0,1})$"); //1-12
    } else {
        rx.setPattern("^([1-9]|[1-2]\\d|3[0-1])$"); //1-31
    }
    m_lineEdit->setValidator(new QRegExpValidator(rx));

    fixup();
}

const QString DateWidget::getCurrentText() const
{
    return m_lineEdit->text();
}

void DateWidget::setCurrentText(QString text)
{
    m_lineEdit->setText(text);
}

bool DateWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_lineEdit && event->type() == QEvent::FocusOut) {
        fixup();
        Q_EMIT editingFinished();
    }

    return false;
}

void DateWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_lineEdit->setFocus();
    }
}

int DateWidget::minimum() const
{
    return m_minimum;
}
