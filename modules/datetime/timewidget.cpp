#include "timewidget.h"

#include <libdui/dthememanager.h>
#include <libdui/libdui_global.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QEvent>
#include <QTime>

using DUI::DThemeManager;

TimeWidget::TimeWidget(QWidget *parent) :
    QFrame(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    m_normalWidget = new NormalWidget;
    m_editWidget = new EditWidget;

    QVBoxLayout *centeralLayout = new QVBoxLayout;
    centeralLayout->addWidget(m_normalWidget);
    centeralLayout->addWidget(m_editWidget);
    centeralLayout->setSpacing(0);
    centeralLayout->setMargin(0);

    setLayout(centeralLayout);

    m_editWidget->hide();

    connect(m_normalWidget, &NormalWidget::onDoubleClick, this, &TimeWidget::switchToEditMode);
    connect(m_editWidget, &EditWidget::accept, this, &TimeWidget::applyTime);
    connect(m_editWidget, &EditWidget::accept, this, &TimeWidget::switchToNormalMode);
    connect(m_editWidget, &EditWidget::cancel, this, &TimeWidget::switchToNormalMode);
}

void TimeWidget::switchToEditMode()
{
    m_normalWidget->hide();
    m_editWidget->show();

    m_editWidget->setTime(QTime::currentTime());
}

void TimeWidget::switchToNormalMode()
{
    m_normalWidget->show();
    m_editWidget->hide();

    m_normalWidget->setTime(QTime::currentTime());
}


NormalWidget::NormalWidget(QWidget *parent) :
    QWidget(parent)
{
    setFixedHeight(150);

    m_timeLabel = new QLabel;
    m_timeLabel->setAlignment(Qt::AlignCenter);
    m_timeLabel->setObjectName("TimeLabel");
    QLabel *normalTipsLabel = new QLabel;
    normalTipsLabel->setAlignment(Qt::AlignCenter);
    normalTipsLabel->setText("asdadsasdasd");
    normalTipsLabel->setObjectName("TipsLabel");

    QVBoxLayout *normalLayout = new QVBoxLayout;
    normalLayout->addWidget(m_timeLabel);
    normalLayout->addWidget(normalTipsLabel);
    normalLayout->setContentsMargins(0, 30, 0, 30);

    setLayout(normalLayout);
    setTime(QTime::currentTime());

    D_THEME_INIT_WIDGET(NormalWidget);
}

void NormalWidget::setTime(const QTime &time)
{
    const QString hour = QString("%1").arg(QString::number(time.hour()), 2, '0');
    const QString minute = QString("%1").arg(QString::number(time.minute()), 2, '0');

    m_timeLabel->setText(QString("%1:%2").arg(hour).arg(minute));
}

void NormalWidget::enterEvent(QEvent *)
{
}

void NormalWidget::mouseDoubleClickEvent(QMouseEvent *)
{
    emit onDoubleClick();
}


EditWidget::EditWidget(QWidget *parent) :
    QWidget(parent)
{

    m_spinHour = new TimeSpinBox;
    m_spinHour->setMaxNumber(23);
    m_spinMinute = new TimeSpinBox;
    m_spinMinute->setMaxNumber(59);
    QLabel *timeSplit = new QLabel(":");
    timeSplit->setObjectName("TimeSpliter");

    QHBoxLayout *timeLayout = new QHBoxLayout;
    timeLayout->addStretch();
    timeLayout->addWidget(m_spinHour);
    timeLayout->addWidget(timeSplit);
    timeLayout->addWidget(m_spinMinute);
    timeLayout->addStretch();
    timeLayout->setSpacing(8);

    m_setTimeButton = new DUI::DTextButton(tr("Set"));
    m_cancelTimeButton = new DUI::DTextButton(tr("Cancel"));
    QLabel *controlSplit = new QLabel("|");
    controlSplit->setObjectName("ControlSpliter");

    QHBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->addStretch();
    controlLayout->addWidget(m_setTimeButton);
    controlLayout->addWidget(controlSplit);
    controlLayout->addWidget(m_cancelTimeButton);
    controlLayout->addStretch();

    QVBoxLayout *centeralLayout = new QVBoxLayout;
    centeralLayout->addLayout(timeLayout);
    centeralLayout->addLayout(controlLayout);
    centeralLayout->setSpacing(10);

    setLayout(centeralLayout);

    connect(m_setTimeButton, &DUI::DTextButton::clicked, [this] () -> void {emit accept(QTime(m_spinHour->text().toInt(), m_spinMinute->text().toInt()));});
    connect(m_cancelTimeButton, &DUI::DTextButton::clicked, this, &EditWidget::cancel);

    D_THEME_INIT_WIDGET(EditWidget);
}

void EditWidget::setTime(const QTime &time)
{
    m_spinHour->setNumber(time.hour());
    m_spinMinute->setNumber(time.minute());
}


TimeSpinBox::TimeSpinBox(QWidget *parent) :
    QFrame(parent)
{
    m_spinLabel = new QLabel;
    m_spinLabel->setFixedSize(80, 80);
    m_spinLabel->setAlignment(Qt::AlignCenter);
    m_spinLabel->setObjectName("SpinLabel");

    m_addBtn = new DUI::DImageButton;
    //m_addBtn->setNormalPic(QPixmap("modules/datetime/icons/arrow_up_normal.png"));
    //m_addBtn->setHoverPic(QPixmap("modules/datetime/icons/arrow_up_hover.png"));
    //m_addBtn->setPressPic(QPixmap("modules/datetime/icons/arrow_up_press.png"));
    m_addBtn->setObjectName("SpinAddBtn");
    //m_addBtn->hide();

    m_subBtn = new DUI::DImageButton;
    //m_subBtn->setNormalPic(QPixmap("modules/datetime/icons/arrow_down_normal.png"));
    //m_subBtn->setHoverPic(QPixmap("modules/datetime/icons/arrow_down_hover.png"));
    //m_subBtn->setPressPic(QPixmap("modules/datetime/icons/arrow_down_press.png"));
    m_subBtn->setObjectName("SpinSubBtn");
    //m_subBtn->hide();

    QVBoxLayout *centeralLayout = new QVBoxLayout;
    centeralLayout->addStretch();
    centeralLayout->addWidget(m_addBtn);
    centeralLayout->setAlignment(m_addBtn, Qt::AlignCenter);
    centeralLayout->addWidget(m_spinLabel);
    centeralLayout->addWidget(m_subBtn);
    centeralLayout->setAlignment(m_subBtn, Qt::AlignCenter);
    centeralLayout->addStretch();
    centeralLayout->setSpacing(0);

    setLayout(centeralLayout);

    connect(m_addBtn, &DUI::DImageButton::clicked, this, &TimeSpinBox::addNumber);
    connect(m_subBtn, &DUI::DImageButton::clicked, this, &TimeSpinBox::subNumber);

    D_THEME_INIT_WIDGET(TimeSpinBox);
}

void TimeSpinBox::setNumber(int num)
{
    m_spinLabel->setText(QString("%1").arg(QString::number(num), 2, QLatin1Char('0')));
}

void TimeSpinBox::addNumber()
{
    const int num = m_spinLabel->text().toInt();

    if (num < m_maxNumber)
        setNumber(num + 1);
    else
        setNumber(m_minNumber);
}

void TimeSpinBox::subNumber()
{
    const int num = m_spinLabel->text().toInt();

    if (num > m_minNumber)
        setNumber(num - 1);
    else
        setNumber(m_maxNumber);
}
