#include "timezonewidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <libdui/dseparatorhorizontal.h>

DUI_USE_NAMESPACE

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

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(btnsWidget);
    hLayout->addLayout(labelsLayout);
    hLayout->addStretch();
    hLayout->setContentsMargins(8, 0, 8, 0);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(hLayout);
    mainLayout->addWidget(new DSeparatorHorizontal);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    setLayout(mainLayout);
    setFixedHeight(50);
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
