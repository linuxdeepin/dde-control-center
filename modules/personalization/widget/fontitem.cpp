#include "fontitem.h"

using namespace dcc;
using namespace dcc::personalization;

FontItem::FontItem(const QString &value, QWidget *parent) :
    QWidget(parent)
{
    m_mainLayout = new QHBoxLayout;
    m_title      = new QLabel;
    QFont font;
    font.setFamily(value);
    m_title->setFont(font);
    m_title->setText(value);
    m_mainLayout->addWidget(m_title);
    setLayout(m_mainLayout);
    setObjectName("FontItem");
}

