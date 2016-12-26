#include "summaryitem.h"

#include <QVBoxLayout>

#include "labels/normallabel.h"
#include "labels/smalllabel.h"

using namespace dcc::widgets;

namespace dcc {
namespace update {

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
