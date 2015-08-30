#include "timezonewidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

TimezoneWidget::TimezoneWidget(const ZoneInfo *info, QWidget *parent) :
    QFrame(parent),
    m_zoneInfo(info)
{
    m_citiesLabel = new QLabel;
    m_citiesLabel->setObjectName("Cities");

    m_utcOffsetLabel = new QLabel;
    m_utcOffsetLabel->setObjectName("UTCOffset");
    //m_utcOffsetLabel->setStyleSheet("color:white;");

    m_removeBtn = new MultiDeleteButton;
    m_selectedBtn = new DImageButton;
    m_selectedBtn->setNormalPic("modules/datetime/icons/select.png");

    QHBoxLayout *btnsLayout = new QHBoxLayout;
    btnsLayout->addWidget(m_removeBtn);
    btnsLayout->setAlignment(m_removeBtn, Qt::AlignCenter);
    btnsLayout->addWidget(m_selectedBtn);
    btnsLayout->setAlignment(m_selectedBtn, Qt::AlignCenter);
    btnsLayout->setMargin(0);
    btnsLayout->setSpacing(0);

    QWidget *btnsWidget = new QWidget;
    btnsWidget->setLayout(btnsLayout);
    btnsWidget->setFixedWidth(30);

    QVBoxLayout *labelsLayout = new QVBoxLayout;
    labelsLayout->addWidget(m_citiesLabel);
    labelsLayout->addWidget(m_utcOffsetLabel);
    labelsLayout->setSpacing(0);
    labelsLayout->setContentsMargins(0, 3, 0, 3);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(btnsWidget);
    mainLayout->addLayout(labelsLayout);
    mainLayout->addStretch();
    mainLayout->setContentsMargins(8, 0, 8, 0);

    setLayout(mainLayout);
    setFixedHeight(48);
    normalMode();
    reloadThemes();

    connect(this, &TimezoneWidget::selectStateChanged, this, &TimezoneWidget::normalMode);
    connect(this, &TimezoneWidget::selectStateChanged, this, &TimezoneWidget::reloadThemes);
}

void TimezoneWidget::setSelected(const bool selected)
{
    const bool changed = m_selected != selected;

    m_selected = selected;

    if (changed)
        emit selectStateChanged(selected);
}
