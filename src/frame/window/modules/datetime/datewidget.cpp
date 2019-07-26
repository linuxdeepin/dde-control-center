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

#include <QIntValidator>
#include <QHBoxLayout>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::datetime;

DateWidget::DateWidget(Type type, int minimum, int maximum, QFrame *parent)
    : SettingsItem(parent),
      m_type(type),
      m_minimum(minimum),
      m_maximum(maximum),
      m_lineEdit(new QLineEdit),
      m_label(new NormalLabel),
      m_addBtn(new DImageButton),
      m_reducedBtn(new DImageButton)
{
    setFixedHeight(36);

    m_lineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    m_lineEdit->setObjectName("DCC-Datetime-QLineEdit");
    m_addBtn->setObjectName("DCC-Datetime-Datewidget-Add");
    m_reducedBtn->setObjectName("DCC-Datetime-Datewidget-Reduce");

    m_label->setParent(m_lineEdit);
    m_label->move(0, 0);
    m_addBtn->setParent(m_lineEdit);
    m_reducedBtn->setParent(m_lineEdit);

    m_addBtn->setNormalPic(":/datetime/themes/light/icons/add_normal.png");
    m_addBtn->setHoverPic(":/datetime/themes/light/icons/add_hover.png");
    m_addBtn->setPressPic(":/datetime/themes/light/icons/add_press.png");

    m_reducedBtn->setNormalPic(":/datetime/themes/light/icons/reduce_normal.png");
    m_reducedBtn->setHoverPic(":/datetime/themes/light/icons/reduce_hover.png");
    m_reducedBtn->setPressPic(":/datetime/themes/light/icons/reduce_press.png");

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

    connect(m_addBtn, &DImageButton::clicked, this, &DateWidget::slotAdd);
    connect(m_reducedBtn, &DImageButton::clicked, this, &DateWidget::slotReduced);

    connect(m_lineEdit, &QLineEdit::editingFinished, [this] {
        fixup();
        Q_EMIT editingFinished();
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

    QIntValidator *validator = new QIntValidator(m_minimum, m_maximum, this);
    m_lineEdit->setValidator(validator);

    fixup();
}

bool DateWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_lineEdit && event->type() == QEvent::FocusOut) {
        fixup();
        Q_EMIT editingFinished();
    }

    return false;
}

int DateWidget::minimum() const
{
    return m_minimum;
}
