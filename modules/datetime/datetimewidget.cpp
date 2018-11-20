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

#include "datetimewidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QWheelEvent>
#include <QDate>
#include <QIntValidator>
#include <QHBoxLayout>

namespace dcc {
namespace datetime {

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


TimeWidget::TimeWidget(QFrame *parent)
    : SettingsItem(parent),
      m_hourEdit(new QLineEdit),
      m_minuteEdit(new QLineEdit)
{
    setFixedHeight(60);

    QFont font;
    font.setPointSizeF(18);
    QTime time = QTime::currentTime();

    m_hourEdit->setContextMenuPolicy(Qt::NoContextMenu);
    m_minuteEdit->setContextMenuPolicy(Qt::NoContextMenu);

    m_hourEdit->setFont(font);
    m_hourEdit->setAlignment(Qt::AlignCenter);
    m_hourEdit->setText(QString::number(time.hour()));
    m_hourEdit->setValidator(new QIntValidator(0,23,this));
    m_hourEdit->setFixedWidth(80);

    m_minuteEdit->setFont(font);
    m_minuteEdit->setAlignment(Qt::AlignCenter);
    m_minuteEdit->setText(QString::number(time.minute()));
    m_minuteEdit->setValidator(new QIntValidator(0,59,this));
    m_minuteEdit->setFixedWidth(80);

    m_hourEdit->setAccessibleName("TimeHour");
    m_minuteEdit->setAccessibleName("TimeMinute");

    QLabel *sep = new QLabel;
    sep->setFont(font);
    sep->setText(":");

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addStretch();
    layout->addWidget(m_hourEdit);
    layout->addSpacing(10);
    layout->addWidget(sep);
    layout->addSpacing(10);
    layout->addWidget(m_minuteEdit);
    layout->addStretch();
    setLayout(layout);

    m_hourEdit->installEventFilter(this);
    m_minuteEdit->installEventFilter(this);

    //    connect(m_hourEdit, SIGNAL(returnPressed()), this, SLOT(setHourText()));
    //    connect(m_hourEdit, SIGNAL(textChanged(QString)), this, SLOT(setHourText(QString)));
    //    connect(m_minuteEdit, SIGNAL(returnPressed()), this, SLOT(setMinuteText()));
    //    connect(m_minuteEdit, SIGNAL(textChanged(QString)), this, SLOT(setMinuteText(QString)));
}

int TimeWidget::hour() const
{
    return m_hourEdit->text().toInt();
}

int TimeWidget::minute() const
{
    return m_minuteEdit->text().toInt();
}

bool TimeWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_hourEdit && event->type() == QEvent::FocusOut) {
        const QString hourText = m_hourEdit->text();
        if (hourText.isEmpty()) {
            m_hourEdit->setText(QString::number(QTime::currentTime().hour()));
        } else if (hourText.count() == 1) {
            m_hourEdit->setText(QString("0%1").arg(hourText));
        }
    } else if (watched == m_minuteEdit && event->type() == QEvent::FocusOut) {
        const QString minutesText = m_minuteEdit->text();
        if (minutesText.isEmpty()) {
            m_minuteEdit->setText(QString::number(QTime::currentTime().minute()));
        } else if (minutesText.count() == 1) {
            m_minuteEdit->setText(QString("0%1").arg(minutesText));
        }
    }

    return false;
}

//void TimeWidget::setHourText()
//{
//    m_hourText = m_hourEdit->text();
//    m_hourEdit->hide();

//    update();
//}

//void TimeWidget::setHourText(const QString &text)
//{
//    m_hourText = text;
//    update();
//}

//void TimeWidget::setMinuteText()
//{
//    m_minuteText = m_minuteEdit->text();
//    m_minuteEdit->hide();

//    update();
//}

//void TimeWidget::setMinuteText(const QString &text)
//{
//    m_minuteText = text;

//    update();
//}

//void TimeWidget::paintEvent(QPaintEvent *)
//{
//    QPainter painter(this);
//    painter.setBrush(Qt::white);

//    painter.fillRect(m_hour, QColor(100,100,100,20));
//    painter.fillRect(m_minute, QColor(100,100,100,20));

//    painter.setFont(m_font);

//    if(!m_hourEdit->isVisible())
//    {
//        painter.drawText(m_hour, Qt::AlignCenter, m_hourText);
//    }

//    painter.drawText(QRect(m_hour.right(), 0, m_minute.left() - m_hour.right(), height()), Qt::AlignCenter, tr(":"));

//    if(!m_minuteEdit->isVisible())
//    {
//        painter.drawText(m_minute, Qt::AlignCenter, m_minuteText);
//    }
//}

//void TimeWidget::mousePressEvent(QMouseEvent *e)
//{
//    if(m_hourEdit->isHidden() && m_hour.contains(e->pos()))
//    {
//        m_hourEdit->setVisible(true);
//        m_hourEdit->setFocus();
//    }
//    else
//    {
//        m_hourEdit->hide();
//    }

//    if(m_minuteEdit->isHidden() && m_minute.contains(e->pos()))
//    {
//        m_minuteEdit->setVisible(true);
//        m_minuteEdit->setFocus();
//    }
//    else
//    {
//        m_minuteEdit->hide();
//    }
//}

//void TimeWidget::wheelEvent(QWheelEvent *e)
//{
//    if(m_hourEdit->isVisible())
//    {
//        QPoint angle = e->angleDelta();
//        int hour = m_hourText.toInt();
//        if(angle.y() == -120)
//        {
//            ++hour;
//        }
//        else
//        {
//            --hour;
//        }
//        hour = (hour > 23) ? 0 : hour;
//        hour = (hour < 0 ) ? 23 : hour;

//        m_hourText.setNum(hour);
//        m_hourEdit->setText(m_hourText);
//    }


//    if(m_minuteEdit->isVisible())
//    {
//        QPoint angle = e->angleDelta();
//        int minute = m_minuteText.toInt();
//        if(angle.y() == -120)
//        {
//            ++minute;
//        }
//        else
//        {
//            --minute;
//        }
//        minute = (minute > 59) ? 0 : minute;
//        minute = (minute < 0 ) ? 59 : minute;

//        m_minuteText.setNum(minute);
//        m_minuteEdit->setText(m_minuteText);
//    }
//}

//void TimeWidget::showEvent(QShowEvent *e)
//{
//    Q_UNUSED(e);

//    m_hour = QRect(0,0,75,height());
//    m_hour.moveRight(this->rect().center().x() - 10);
//    m_minute = QRect(width()-75, 0, 75, height());
//    m_minute.moveLeft(this->rect().center().x() + 10);
//    m_hourEdit->setGeometry(m_hour);
//    m_minuteEdit->setGeometry(m_minute);
//}

}
}
