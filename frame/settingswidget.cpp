#include "settingswidget.h"

#include "accounts/accounts.h"
#include "display/display.h"

#include <QVBoxLayout>
#include <QResizeEvent>

SettingsWidget::SettingsWidget(QWidget *parent)
    : ContentWidget(parent)
{
    QVBoxLayout *settingsLayout = new QVBoxLayout;
    settingsLayout->addWidget(new Accounts);
    settingsLayout->addWidget(new Display);
    settingsLayout->setSpacing(0);
    settingsLayout->setMargin(0);

    m_settingsWidget = new QWidget;
    m_settingsWidget->setLayout(settingsLayout);
    setContent(m_settingsWidget);
}
