#include "updatewidget.h"
#include "separatorwidget.h"
#include "constants.h"

#include <QVBoxLayout>

#include <libdui/dseparatorhorizontal.h>

UpdateWidget::UpdateWidget(QWidget *parent)
    : QWidget(parent)
{
    m_updateCountTips = new QLabel;
    m_updateSizeTips = new QLabel;
    m_updateProgress = new DCircleProgress;
    m_updateProgress->setFixedSize(35, 35);

#ifdef QT_DEBUG
    m_updateCountTips->setText("You have 2 softwares need update");
    m_updateSizeTips->setText("Total update size: 130M");
#endif

    QVBoxLayout *tipsLayout = new QVBoxLayout;
    tipsLayout->addWidget(m_updateCountTips);
    tipsLayout->addWidget(m_updateSizeTips);
    tipsLayout->setSpacing(0);
    tipsLayout->setMargin(0);

    QHBoxLayout *updateInfoLayout = new QHBoxLayout;
    updateInfoLayout->addLayout(tipsLayout);
    updateInfoLayout->addStretch();
    updateInfoLayout->addWidget(m_updateProgress);
    updateInfoLayout->setSpacing(0);
    updateInfoLayout->setMargin(0);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(updateInfoLayout);
    mainLayout->addWidget(new HSeparatorWidget);
    mainLayout->addStretch();
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    setLayout(mainLayout);
//    setFixedWidth(DCC::ModuleContentWidth);
    loadAppList();
}

void UpdateWidget::loadAppList()
{
    setFixedHeight(200);
//    updateGeometry();
}

