#include "settingswidget.h"
#include "modulewidget.h"
#include "frame.h"
#include "moduleinitthread.h"

#include "accounts/accountsmodule.h"
#include "power/powermodule.h"
#include "update/updatemodule.h"
#include "sound/soundmodule.h"
#include "datetime/datetimemodule.h"
#include "defapp/defaultappsmodule.h"
#include "bluetooth/bluetoothmodule.h"

#include <QVBoxLayout>
#include <QResizeEvent>
#include <QScrollBar>

SettingsWidget::SettingsWidget(Frame *frame)
    : ContentWidget(frame),

      m_frame(frame),

      m_settingsLayout(new QVBoxLayout),
      m_settingsWidget(new QWidget),

      m_refershModuleActivableTimer(new QTimer(this))
{
    m_settingsLayout->setSpacing(30);
    m_settingsLayout->setMargin(0);

    loadModule(new AccountsModule(this));
    loadModule(new PowerModule(this));
    loadModule(new UpdateModule(this));
    loadModule(new BluetoothModule(this));
    loadModule(new SoundModule(this));
    loadModule(new DatetimeModule(this));
    loadModule(new DefaultAppsModule(this));

    m_settingsWidget->setLayout(m_settingsLayout);

    setContent(m_settingsWidget);
    setTitle(tr("All Settings"));

    m_refershModuleActivableTimer->setSingleShot(true);
    m_refershModuleActivableTimer->setInterval(100);

    connect(m_contentArea->verticalScrollBar(), &QScrollBar::valueChanged, m_refershModuleActivableTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(m_refershModuleActivableTimer, &QTimer::timeout, this, &SettingsWidget::refershModuleActivable);
}

void SettingsWidget::contentPopuped(ContentWidget * const w)
{
    QMap<ModuleInterface *, QList<ContentWidget *>>::iterator it =
            std::find_if(m_moduleWidgets.begin(), m_moduleWidgets.end(),
                         [=] (const QList<ContentWidget *> &list) {
        return !list.isEmpty() && list.back() == w;
    });

    Q_ASSERT(it != m_moduleWidgets.end());

    it.key()->contentPopped(w);
    it.value().pop_back();
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

    m_moduleInterfaces.append(module);
    m_moduleWidgets.insert(module, QList<ContentWidget *>());

    ModuleInitThread *thrd = new ModuleInitThread(module, this);
    connect(thrd, &ModuleInitThread::moduleInitFinished, this, &SettingsWidget::onModuleInitFinished);
    connect(thrd, &ModuleInitThread::finished, thrd, &ModuleInitThread::deleteLater);
    connect(thrd, &ModuleInitThread::finished, m_refershModuleActivableTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    thrd->start();
}

void SettingsWidget::onModuleInitFinished(ModuleInterface * const module)
{
    Q_ASSERT(m_moduleInterfaces.contains(module));

    // get right position to insert
    int index = 0;
    for (int i(0); i != m_moduleInterfaces.size(); ++i)
    {
        if (m_moduleInterfaces[i] == module)
            break;

        if (m_moduleActivable.contains(m_moduleInterfaces[i]))
            ++index;
    }

    m_moduleActivable[module] = false;
    m_settingsLayout->insertWidget(index, module->moduleWidget());
}

void SettingsWidget::refershModuleActivable()
{
    const QRect containerRect = QRect(QPoint(), m_contentArea->size());

    for (ModuleInterface *module : m_moduleInterfaces)
    {
        const QWidget *w = module->moduleWidget();
        const QRect wRect(w->mapTo(m_contentArea, QPoint()), w->size());
        const bool activable = containerRect.intersects(wRect);

        if (m_moduleActivable[module] == activable)
            continue;

        m_moduleActivable[module] = activable;
        if (activable)
            module->moduleActive();
        else
            module->moduleDeactive();
    }
}

SettingsWidget::~SettingsWidget()
{
    qDeleteAll(m_moduleInterfaces);
}
