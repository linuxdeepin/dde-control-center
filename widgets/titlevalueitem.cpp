#include "titlevalueitem.h"
#include <QHBoxLayout>

#include "labels/tipslabel.h"

namespace dcc
{

namespace widgets
{

TitleValueItem::TitleValueItem(QFrame *parent)
    : SettingsItem(parent),
      m_title(new TipsLabel),
      m_value(new TipsLabel)
{
    QHBoxLayout* layout = new QHBoxLayout;

    m_value->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_value->setWordWrap(true);

    layout->setContentsMargins(20, 9, 10, 10);

    layout->addWidget(m_title);
    layout->addWidget(m_value);

    setLayout(layout);
//    setFixedHeight(36);
}

void TitleValueItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

void TitleValueItem::setValue(const QString &value)
{
    m_value->setText(value);
}

}

}
