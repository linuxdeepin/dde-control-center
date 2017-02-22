#include "settingsheaderitem.h"

#include "labels/normallabel.h"

namespace dcc {
namespace widgets {

SettingsHeaderItem::SettingsHeaderItem(QWidget *parent)
    : SettingsItem(parent),
      m_mainLayout(new QHBoxLayout),
      m_headerText(new NormalLabel)
{
    m_headerText->setObjectName("SettingsHeaderItemTitle");

    m_mainLayout->addSpacing(20);
    m_mainLayout->addWidget(m_headerText);
    m_mainLayout->addStretch();

    setFixedHeight(36);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setMargin(0);

    setLayout(m_mainLayout);
}

void SettingsHeaderItem::setTitle(const QString &title)
{
    m_headerText->setText(title);
}

}
}
