#include "settingsheaderitem.h"

using namespace dcc;

SettingsHeaderItem::SettingsHeaderItem(QWidget *parent)
    : SettingsItem(parent),

      m_mainLayout(new QHBoxLayout),
      m_headerIcon(new QLabel),
      m_headerText(new QLabel)
{
    m_mainLayout->addWidget(m_headerIcon);
    m_mainLayout->addWidget(m_headerText);
    m_mainLayout->addStretch();

    setLayout(m_mainLayout);
}

void SettingsHeaderItem::setIcon(const QPixmap &icon)
{
    m_headerIcon->setPixmap(icon);
}

void SettingsHeaderItem::setTitle(const QString &title)
{
    m_headerText->setText(title);
}
