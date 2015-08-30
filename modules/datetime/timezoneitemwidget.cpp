#include "timezoneitemwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <libdui/dseparatorhorizontal.h>

DUI_USE_NAMESPACE

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

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addLayout(labelsLayout);
    hLayout->addStretch();
    hLayout->addWidget(m_checkBtn);
    hLayout->setSpacing(0);
    hLayout->setContentsMargins(8, 3, 8, 3);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(hLayout);
    mainLayout->addWidget(new DSeparatorHorizontal);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    setLayout(mainLayout);
    setFixedHeight(50);
    reloadThemes();
}

