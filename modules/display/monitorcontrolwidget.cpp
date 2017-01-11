#include "monitorcontrolwidget.h"
#include "monitorsground.h"
#include "displaymodel.h"
#include "monitorproxywidget.h"

#include <QVBoxLayout>
#include <QFrame>
#include <QPushButton>

namespace dcc {

namespace display {

MonitorControlWidget::MonitorControlWidget(QWidget *parent)
    : QFrame(parent),

      m_screensGround(new MonitorsGround),
      m_recognize(new QPushButton),
      m_split(new QPushButton),
      m_join(new QPushButton)
{
    m_recognize->setText(tr("Recognize"));
    m_recognize->setCursor(Qt::PointingHandCursor);
    m_recognize->setObjectName("Recognize");
    m_split->setText(tr("Split"));
    m_split->setCursor(Qt::PointingHandCursor);
    m_split->setObjectName("Split");
    m_join->setText(tr("Merge"));
    m_join->setCursor(Qt::PointingHandCursor);
    m_join->setObjectName("Merge");

    QHBoxLayout *btnsLayout = new QHBoxLayout;
    btnsLayout->addStretch();
    btnsLayout->addWidget(m_recognize);
    btnsLayout->addWidget(m_split);
    btnsLayout->addWidget(m_join);
    btnsLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_screensGround);
    mainLayout->setAlignment(m_screensGround, Qt::AlignHCenter);
    mainLayout->addLayout(btnsLayout);

    setLayout(mainLayout);

    connect(m_recognize, &QPushButton::clicked, this, &MonitorControlWidget::requestRecognize);
    connect(m_join, &QPushButton::clicked, this, &MonitorControlWidget::requestMerge);
    connect(m_split, &QPushButton::clicked, this, &MonitorControlWidget::requestSplit);
    connect(m_screensGround, &MonitorsGround::requestApplySettings, this, &MonitorControlWidget::requestSetMonitorPosition);
}

void MonitorControlWidget::setDisplayModel(DisplayModel *model)
{
    m_screensGround->setDisplayModel(model);
}

void MonitorControlWidget::setScreensMerged(const bool merged)
{
    m_split->setVisible(merged);
    m_join->setVisible(!merged);
}

} // namespace display

} // namespace dcc

