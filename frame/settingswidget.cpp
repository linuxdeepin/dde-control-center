#include "settingswidget.h"
#include "modulewidget.h"
#include "frame.h"

//#include "accounts/accounts.h"
//#include "display/display.h"
//#include "datetime/datetime.h"

#include "accounts/accountsmodule.h"

#include <QVBoxLayout>
#include <QResizeEvent>

SettingsWidget::SettingsWidget(Frame *frame)
    : ContentWidget(frame),

      m_frame(frame),

      m_settingsLayout(new QVBoxLayout),
      m_settingsWidget(new QWidget)
{
//    QVBoxLayout *settingsLayout = new QVBoxLayout;
//    settingsLayout->addWidget(new Accounts(frame));
//    settingsLayout->addWidget(new Display(frame));
//    settingsLayout->addWidget(new Datetime(frame));
//    settingsLayout->setSpacing(0);
//    settingsLayout->setMargin(0);
    m_settingsLayout->setSpacing(0);
    m_settingsLayout->setMargin(0);

    loadModule(new AccountsModule(this));

    m_settingsWidget = new QWidget;
    m_settingsWidget->setLayout(m_settingsLayout);
    setContent(m_settingsWidget);
    setTitle(tr("All Settings"));
}

void SettingsWidget::contentPopuped(ContentWidget * const w)
{
    QMap<ModuleInterface *, QList<ContentWidget *>>::iterator it =
            std::find_if(m_moduleWidgets.begin(), m_moduleWidgets.end(),
                         [=] (const QList<ContentWidget *> &list) {
        return !list.isEmpty() && list.back() == w;
    });

    if (it != m_moduleWidgets.end())
    {
        it.key()->contentPopped(w);
        it.value().pop_back();
    }
}

void SettingsWidget::pushWidget(ModuleInterface * const inter, ContentWidget * const w)
{
    Q_ASSERT(!m_moduleWidgets[inter].contains(w));

    m_moduleWidgets[inter].append(w);
    m_frame->pushWidget(w);
}

void SettingsWidget::loadModule(ModuleInterface * const module)
{
    Q_ASSERT(!m_moduleInterfaces.contains(module));
    Q_ASSERT(!m_moduleWidgets.contains(module));

    module->initialize();
    qDebug() << "load Module" << module->name();

    m_moduleInterfaces.append(module);
    m_moduleWidgets.insert(module, QList<ContentWidget *>());

    // add widget
    ModuleWidget *w = module->moduleWidget();
    Q_ASSERT(w);

    m_settingsLayout->addWidget(w);
}
