#include "settingsoptionitem.h"

#include <QHBoxLayout>

SettingsOptionItem::SettingsOptionItem(QWidget *parent)
    : QWidget(parent),

      m_title(new QLabel),
      m_icon(new QLabel)
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_title);
    mainLayout->addWidget(m_icon);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    setLayout(mainLayout);
}

void SettingsOptionItem::setTitle(const QString &title)
{
    m_title->setText(title);
}
