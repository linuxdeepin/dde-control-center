#include "summaryitem.h"

#include <QVBoxLayout>
#include <QTimer>

#include "labels/normallabel.h"
#include "labels/smalllabel.h"
#include "loadingindicator.h"

using namespace dcc::widgets;

namespace dcc {
namespace update {

CheckUpdateItem::CheckUpdateItem(QFrame *parent)
    : SettingsItem(parent),
      m_messageLabel(new NormalLabel),
      m_indicator(new LoadingIndicator)
{
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addStretch();
    layout->addWidget(m_indicator, 0, Qt::AlignHCenter);
    layout->addWidget(m_messageLabel, 0, Qt::AlignHCenter);
    layout->addStretch();

    setLayout(layout);
    setFixedHeight(100);

    QTimer::singleShot(0, m_indicator, &LoadingIndicator::play);
}

void CheckUpdateItem::setIndicatorVisible(bool visible)
{
    m_indicator->setVisible(visible);
}

void CheckUpdateItem::setMessage(const QString &message)
{
    m_messageLabel->setText(message);
}

SummaryItem::SummaryItem(QFrame *parent)
    :SettingsItem(parent),
      m_title(new NormalLabel),
      m_details(new SmallLabel)
{
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    m_title->setAlignment(Qt::AlignCenter);
    m_details->hide();

    layout->addWidget(m_title);
    layout->addWidget(m_details);

    setLayout(layout);

    setFixedHeight(64);
}

void SummaryItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

void SummaryItem::setDetails(const QString &details)
{
    m_title->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);
    m_details->setAlignment(Qt::AlignTop|Qt::AlignHCenter);
    m_details->setText(details);
    m_details->show();
}

}
}
