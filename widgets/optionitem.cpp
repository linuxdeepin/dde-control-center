#include "optionitem.h"

using namespace dcc;

OptionItem::OptionItem(QWidget *parent)
    : SettingsItem(parent),

      m_mainLayout(new QHBoxLayout),
      m_title(new NormalLabel),
      m_selectedIcon(new QLabel)
{
    m_mainLayout->addWidget(m_title);
    m_mainLayout->addWidget(m_selectedIcon);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(m_mainLayout);
}

void OptionItem::setTitle(const QString &title)
{
    m_title->setText(title);
}
