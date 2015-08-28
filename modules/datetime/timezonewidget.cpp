#include "timezonewidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <libdui/dthememanager.h>

DUI_USE_NAMESPACE

TimezoneWidget::TimezoneWidget(QWidget *parent) : QFrame(parent)
{
    m_citiesLabel = new QLabel;
    m_citiesLabel->setObjectName("Cities");
    m_citiesLabel->setStyleSheet("color:white;");

    m_utcOffsetLabel = new QLabel;
    m_utcOffsetLabel->setObjectName("UTCOffset");
    m_utcOffsetLabel->setStyleSheet("color:white;");

    QVBoxLayout *labelsLayout = new QVBoxLayout;
    labelsLayout->addWidget(m_citiesLabel);
    labelsLayout->addWidget(m_utcOffsetLabel);
    labelsLayout->setMargin(0);

    m_removeBtn = new MultiDeleteButton;
    m_selectedBtn = new DImageButton;
    m_selectedBtn->setNormalPic("modules/datetime/icons/select.png");
    m_chooseBtn = new MultiAddCheckButton;

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_removeBtn);
    mainLayout->addWidget(m_selectedBtn);
    mainLayout->addLayout(labelsLayout);
    mainLayout->addStretch();
    mainLayout->addWidget(m_chooseBtn);
    mainLayout->setMargin(0);

    setLayout(mainLayout);
    setFixedHeight(40);

    D_THEME_INIT_WIDGET(TimezoneWidget);
}

