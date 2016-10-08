#include "settingswidget.h"

#include "accounts/accounts.h"

#include <QVBoxLayout>

SettingsWidget::SettingsWidget(QWidget *parent)
    : ContentWidget(parent)
{
    QVBoxLayout *settingsLayout = new QVBoxLayout;
    settingsLayout->addWidget(new Accounts);

    QWidget *settingsWidget = new QWidget;
    settingsWidget->setLayout(settingsLayout);
    m_contentArea->setWidget(settingsWidget);
}
