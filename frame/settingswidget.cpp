#include "settingswidget.h"
#include "frame.h"

#include "accounts/accounts.h"
#include "display/display.h"

#include <QVBoxLayout>
#include <QResizeEvent>

SettingsWidget::SettingsWidget(Frame *frame)
    : ContentWidget(frame)
{
    QVBoxLayout *settingsLayout = new QVBoxLayout;
    settingsLayout->addWidget(new Accounts(frame));
    settingsLayout->addWidget(new Display(frame));
    settingsLayout->setSpacing(0);
    settingsLayout->setMargin(0);

    m_settingsWidget = new QWidget;
    m_settingsWidget->setLayout(settingsLayout);
    setContent(m_settingsWidget);
    setTitle(tr("All Settings"));
}
