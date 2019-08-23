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
#include "timewidget.h"

#include <QTime>
#include <QIntValidator>
#include <QDebug>

using namespace dcc::widgets;

namespace DCC_NAMESPACE {
namespace datetime {

//bType : true -> hour , false -> minute
TimeWidget::TimeWidget(QFrame *parent, bool bType)
    : SettingsItem(parent)
    , m_lineEdit(new QLineEdit)
    , m_addBtn(new DImageButton)
    , m_reducedBtn(new DImageButton)
    , m_btnLayout(new QVBoxLayout)
    , m_layout(new QHBoxLayout)
    , m_value(0)
    , m_bType(bType)
{
    QFont font;
    font.setPointSizeF(18);
    QTime time = QTime::currentTime();

    m_lineEdit->installEventFilter(this);

    m_lineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    m_lineEdit->setFont(font);
    m_lineEdit->setAlignment(Qt::AlignCenter);
    m_lineEdit->setText(QString::number(time.hour()));
    m_lineEdit->setValidator(new QIntValidator(0, 23, this));
    m_lineEdit->setFixedWidth(60);
//    m_lineEdit->setAccessibleName("TimeHour");

    m_addBtn->setNormalPic(":/datetime/themes/light/icons/add_normal.png");
    m_addBtn->setHoverPic(":/datetime/themes/light/icons/add_hover.png");
    m_addBtn->setPressPic(":/datetime/themes/light/icons/add_press.png");

    m_reducedBtn->setNormalPic(":/datetime/themes/light/icons/reduce_normal.png");
    m_reducedBtn->setHoverPic(":/datetime/themes/light/icons/reduce_hover.png");
    m_reducedBtn->setPressPic(":/datetime/themes/light/icons/reduce_press.png");

    m_layout->addWidget(m_lineEdit);
    m_btnLayout->addWidget(m_addBtn);
    m_btnLayout->addWidget(m_reducedBtn);
    m_layout->addSpacing(0);
    m_layout->addStretch();
    m_layout->addLayout(m_btnLayout);
    setLayout(m_layout);

    if (m_bType) {
        m_lineEdit->setText(QString::number(time.hour()));
        m_lineEdit->setValidator(new QIntValidator(0, 23, this));
    } else {
        m_lineEdit->setText(QString::number(time.minute()));
        m_lineEdit->setValidator(new QIntValidator(0, 59, this));
    }

    m_value = m_lineEdit->text().toInt();

    connect(m_addBtn, &DImageButton::clicked, this, &TimeWidget::slotAdd);
    connect(m_reducedBtn, &DImageButton::clicked, this, &TimeWidget::slotReduced);
    connect(m_lineEdit, &QLineEdit::editingFinished, this, &TimeWidget::slotSetLineEdit);
}

int TimeWidget::getEditValue() const
{
    return m_lineEdit->text().toInt();
}

void TimeWidget::setEditText(QString txt)
{
    if (m_lineEdit) {
        m_lineEdit->setText(txt);
    }
}

void TimeWidget::slotAdd()
{
    if (m_bType) { // hour
        if (m_value >= 0 && m_value < 23) {
            m_value++;
        } else if (m_value >= 23) {
            m_value = 0;
        } else {
            qDebug() << "input hour error";
        }
    } else { //minute
        if (m_value >= 0 && m_value < 59) {
            m_value++;
        } else if (m_value >= 59) {
            m_value = 0;
        } else {
            qDebug() << "input hour error";
        }
    }

    m_lineEdit->setText(QString::number(m_value));
}

void TimeWidget::slotReduced()
{
    if (m_bType) { // hour
        if (m_value <= 0) {
            m_value = 23;
        } else if (m_value > 0 && m_value <= 23) {
            m_value--;
        } else {
            qDebug() << "input hour error";
        }
    } else { //minute
        if (m_value <= 0) {
            m_value = 59;
        } else if (m_value > 0 && m_value <= 59) {
            m_value--;
        } else {
            qDebug() << "input hour error";
        }
    }

    m_lineEdit->setText(QString::number(m_value));
}

void TimeWidget::slotSetLineEdit()
{
    m_value = m_lineEdit->text().toInt();
}

//add 0
bool TimeWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (m_bType) {
        if (watched == m_lineEdit && event->type() == QEvent::FocusOut) {
            const QString hourText = m_lineEdit->text();
            if (hourText.isEmpty()) {
                m_lineEdit->setText(QString::number(QTime::currentTime().hour()));
            } else if (hourText.count() == 1) {
                m_lineEdit->setText(QString("0%1").arg(hourText));
            }
        }
    } else {
        if (watched == m_lineEdit && event->type() == QEvent::FocusOut) {
            const QString minutesText = m_lineEdit->text();
            if (minutesText.isEmpty()) {
                m_lineEdit->setText(QString::number(QTime::currentTime().minute()));
            } else if (minutesText.count() == 1) {
                m_lineEdit->setText(QString("0%1").arg(minutesText));
            }
        }
    }

    return false;
}

}// namespace datetime
}// namespace DCC_NAMESPACE
