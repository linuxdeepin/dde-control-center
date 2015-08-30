#include "timezoneitemwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

TimezoneItemWidget::TimezoneItemWidget(const ZoneInfo *info, QWidget *parent) :
    QFrame(parent),
    m_zoneInfo(info)
{
    m_zones = new QLabel;
    m_zones->setObjectName("Zones");

    m_utcOffset = new QLabel;
    m_utcOffset->setObjectName("UTCOffset");

    QVBoxLayout *labelsLayout = new QVBoxLayout;
    labelsLayout->addWidget(m_zones);
    labelsLayout->addWidget(m_utcOffset);
    labelsLayout->setMargin(0);
    labelsLayout->setSpacing(0);

    m_checkBtn = new MultiAddCheckButton;
    m_checkBtn->setCheckable(true);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addLayout(labelsLayout);
    mainLayout->addStretch();
    mainLayout->addWidget(m_checkBtn);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(8, 3, 8, 3);

    setLayout(mainLayout);
    setFixedHeight(48);
    reloadThemes();
}

