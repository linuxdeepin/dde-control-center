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

DateWidget::DateWidget(Type type, QFrame *parent)
    : SettingsItem(parent),
      m_type(type),
      m_lineEdit(new QLineEdit),
      m_label(new NormalLabel),
      m_addBtn(new DImageButton),
      m_reducedBtn(new DImageButton)
{
    setFixedHeight(36);

    m_lineEdit->setObjectName("DCC-Datetime-QLineEdit");
    m_addBtn->setObjectName("DCC-Datetime-Datewidget-Add");
    m_reducedBtn->setObjectName("DCC-Datetime-Datewidget-Reduce");

    QDate date = QDate::currentDate();
    if(m_type == Year) {
        m_label->setText(tr("Year"));
        m_minimum = 1;
        m_maximum = 9999;
        m_lineEdit->setText(QString::number(date.year()));
    } else if(m_type == Month) {
        m_label->setText(tr("Month"));
        m_minimum = 1;
        m_maximum = 12;
        m_lineEdit->setText(QString::number(date.month()));
    } else {
        m_label->setText(tr("Day"));
        m_minimum = 1;
        m_maximum = 31;
        m_lineEdit->setText(QString::number(date.day()));
    }

    m_lineEdit->setFixedWidth(40);
    m_lineEdit->setAlignment(Qt::AlignCenter);
    m_lineEdit->setValidator(new QIntValidator(m_minimum, m_maximum, this));

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
}

int DateWidget::data() const
{
    return m_lineEdit->text().toInt();
}

void DateWidget::slotAdd()
{
    int value = m_lineEdit->text().toInt() + 1;
    if (m_minimum <= value && value <= m_maximum) {
        m_lineEdit->setText(QString::number(value));
    }
}

void DateWidget::slotReduced()
{
    int value = m_lineEdit->text().toInt() - 1;
    if (m_minimum <= value && value <= m_maximum) {
        m_lineEdit->setText(QString::number(value));
    }
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
        if (m_hourEdit->text().isEmpty()) {
            m_hourEdit->setText(QString::number(QTime::currentTime().hour()));
        }
    } else if (watched == m_minuteEdit && event->type() == QEvent::FocusOut) {
        if (m_minuteEdit->text().isEmpty()) {
            m_minuteEdit->setText(QString::number(QTime::currentTime().minute()));
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
