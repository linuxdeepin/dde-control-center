#include "titlevalueitem.h"
#include <QHBoxLayout>

namespace dcc
{

namespace widgets
{

TitleValueItem::TitleValueItem(QFrame *parent)
    :SettingsItem(parent)
{
    QHBoxLayout* layout = new QHBoxLayout;

    m_title = new QLabel;
    m_value = new QLabel;
    m_value->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_value->setWordWrap(true);

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
