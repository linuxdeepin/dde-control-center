#include "summaryitem.h"

#include <QVBoxLayout>

#include "labels/normallabel.h"
#include "labels/tipslabel.h"

using namespace dcc::widgets;

namespace dcc {
namespace update {

SummaryItem::SummaryItem(QFrame *parent)
    :SettingsItem(parent),
      m_title(new NormalLabel),
      m_details(new TipsLabel)
{
    setFixedHeight(36 * 3);

    m_title->setObjectName("UpdateSummary");

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    m_title->setAlignment(Qt::AlignHCenter);
    m_title->setWordWrap(true);
    m_details->setAlignment(Qt::AlignHCenter);

    layout->addStretch();
    layout->addWidget(m_title);
    layout->addSpacing(4);
    layout->addWidget(m_details);
    layout->addStretch();

    setLayout(layout);
}

void SummaryItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

void SummaryItem::setDetails(const QString &details)
{
    m_details->setText(details);
}

}
}
