#include "settingswidget.h"
#include "frame.h"
#include "moduleinitthread.h"
#include "modulewidget.h"

#include "accounts/accountsmodule.h"
#include "bluetooth/bluetoothmodule.h"
#include "datetime/datetimemodule.h"
#include "defapp/defaultappsmodule.h"
#include "keyboard/keyboardmodule.h"
#include "power/powermodule.h"
#include "sound/soundmodule.h"
#include "update/updatemodule.h"
#include "mouse/mousemodule.h"
#include "wacom/wacomemodule.h"
#include "display/displaymodule.h"
#include "personalization/personalizationmodule.h"
#include "systeminfo/systeminfomodule.h"
#include "network/networkmodule.h"

#include <QResizeEvent>
#include <QScrollBar>
#include <QVBoxLayout>

SettingsWidget::SettingsWidget(Frame *frame)
    : ContentWidget(frame),

      m_frame(frame),

      m_resetBtn(new QPushButton),
      m_settingsLayout(new QVBoxLayout),
      m_settingsWidget(new TranslucentFrame),

      m_refershModuleActivableTimer(new QTimer(this)),

      m_moduleLoadDelay(0),
      m_loadFinished(false)
{
    m_resetBtn->setText(tr("Reset all settings"));

    m_settingsLayout->addWidget(m_resetBtn);
    m_settingsLayout->setSpacing(30);
    m_settingsLayout->setMargin(0);

    loadModule(new accounts::AccountsModule(this));
    loadModule(new display::DisplayModule(this));
    loadModule(new DefaultAppsModule(this));
    loadModule(new personalization::PersonalizationModule(this));
    loadModule(new network::NetworkModule(this));
    loadModule(new bluetooth::BluetoothModule(this));
    loadModule(new sound::SoundModule(this));
    loadModule(new DatetimeModule(this));
    loadModule(new PowerModule(this));
    loadModule(new MouseModule(this));
    loadModule(new KeyboardModule(this));
    loadModule(new WacomModule(this));
    loadModule(new UpdateModule(this));
    loadModule(new SystemInfoModule(this));

    m_settingsWidget->setLayout(m_settingsLayout);

    setContent(m_settingsWidget);
    setTitle(tr("All Settings"));

    m_refershModuleActivableTimer->setSingleShot(true);
    m_refershModuleActivableTimer->setInterval(100);

    connect(m_resetBtn, &QPushButton::clicked, this, &SettingsWidget::resetAllSettings);
    connect(m_contentArea->verticalScrollBar(), &QScrollBar::valueChanged, m_refershModuleActivableTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(m_refershModuleActivableTimer, &QTimer::timeout, this, &SettingsWidget::refershModuleActivable);
}

void SettingsWidget::contentPopuped(ContentWidget *const w)
{
    QMap<ModuleInterface *, QList<ContentWidget *>>::iterator it =
        std::find_if(m_moduleWidgets.begin(), m_moduleWidgets.end(),
                     [=](const QList<ContentWidget *> &list) {
                         return !list.isEmpty() && list.back() == w;
                     });

    Q_ASSERT(it != m_moduleWidgets.end());

    it.key()->contentPopped(w);
    it.value().pop_back();
}

void SettingsWidget::setFrameAutoHide(ModuleInterface *const inter, const bool autoHide)
{
    Q_ASSERT(m_moduleInterfaces.contains(inter));

    emit requestAutohide(autoHide);
}

void SettingsWidget::pushWidget(ModuleInterface *const inter, ContentWidget *const w)
{
    Q_ASSERT(!m_moduleWidgets[inter].contains(w));

    m_moduleWidgets[inter].append(w);
    m_frame->pushWidget(w);
}

void SettingsWidget::loadModule(ModuleInterface *const module)
{
    Q_ASSERT(!m_moduleInterfaces.contains(module));
    Q_ASSERT(!m_moduleWidgets.contains(module));

    qDebug() << "load module: " << module->name();

    m_moduleInterfaces.append(module);
    m_moduleWidgets.insert(module, QList<ContentWidget *>());

    ModuleInitThread *thrd = new ModuleInitThread(module, this);
    connect(thrd, &ModuleInitThread::moduleInitFinished, this, &SettingsWidget::onModuleInitFinished);
    connect(thrd, &ModuleInitThread::finished, thrd, &ModuleInitThread::deleteLater);
    QTimer::singleShot(m_moduleLoadDelay, [=] { thrd->start(QThread::LowPriority); });

    m_moduleLoadDelay += 50;
    m_moduleLoadDelay %= 400;
}

void SettingsWidget::onModuleInitFinished(ModuleInterface *const module)
{
    Q_ASSERT(m_moduleInterfaces.contains(module));

    qDebug() << "module initialize: " << module->name();

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

    qDebug() << "load module finished: " << module->name();

    // load all modules finished
    if (m_moduleActivable.size() == m_moduleInterfaces.size())
    {
        m_loadFinished = true;
        m_refershModuleActivableTimer->start();

        // scroll to dest widget
        if (!m_ensureVisibleModule.isEmpty())
            QTimer::singleShot(10, this, [=] { showModulePage(m_ensureVisibleModule, m_ensureVisiblePage); });
    }
}

void SettingsWidget::ensureModuleVisible(const QString &moduleName)
{
    ModuleInterface *inter = nullptr;
    for (auto it : m_moduleInterfaces)
    {
        if (it->name() == moduleName)
        {
            inter = it;
            break;
        }
    }

    if (inter)
        scrollToWidget(inter->moduleWidget());
}

void SettingsWidget::showModulePage(const QString &moduleName, const QString &pageName)
{
    m_ensureVisibleModule = moduleName;
    m_ensureVisiblePage = pageName;

    if (!m_loadFinished)
        return;

    if (pageName.isEmpty())
        ensureModuleVisible(moduleName);
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

void SettingsWidget::resetAllSettings()
{
    for (auto *inter : m_moduleInterfaces)
        inter->reset();
}

SettingsWidget::~SettingsWidget()
{
    qDeleteAll(m_moduleInterfaces);
}
