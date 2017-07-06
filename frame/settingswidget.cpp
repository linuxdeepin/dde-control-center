#include "settingswidget.h"
#include "frame.h"
#include "moduleinitthread.h"
#include "modulewidget.h"
#include "navgationdelegate.h"

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
#include <QScrollArea>
#include <QScroller>

using namespace dcc::datetime;
using namespace dcc::keyboard;
using namespace dcc::systeminfo;
using namespace dcc::update;

SettingsWidget::SettingsWidget(Frame *frame)
    : ContentWidget(frame),

      m_frame(frame),

      m_resetBtn(new QPushButton),
      m_settingsLayout(new QVBoxLayout),
      m_settingsWidget(new TranslucentFrame),

      m_refershModuleActivableTimer(new QTimer(this)),

      m_moduleLoadDelay(0)
{
    qDebug() << Q_FUNC_INFO << "I'm born!!!!";

    m_resetBtn->setText(tr("Reset all settings"));
    m_navgationBtn->setVisible(true);

    m_settingsLayout->setSpacing(30);
    m_settingsLayout->setMargin(0);
    m_settingsLayout->addSpacing(10);
//    m_settingsLayout->addWidget(m_resetBtn);
    m_settingsLayout->addSpacing(20);

    m_navModel = new NavgationModel;

    m_settingsWidget->setLayout(m_settingsLayout);

    m_navTips = new NormalLabel;
    m_navTips->setAlignment(Qt::AlignCenter);
    // view item width
    m_navTips->setFixedWidth(110 * 3 + 2);
#ifdef QT_DEBUG
    m_navTips->setText("Nav Title");
    m_navTips->setStyleSheet("background-color: red;");
#endif

    m_navView = new NavgationView;
    m_navView->setItemDelegate(new NavgationDelegate);
    m_navView->setModel(m_navModel);
    m_navView->setFixedWidth(110 * 3 + 5);
//    m_navView->setParent(this);
//    m_navView->move(14, 0);
//    m_navView->setFixedSize(FRAME_WIDTH, 600);
//    m_navView->setVisible(false);

    QVBoxLayout *navLayout = new QVBoxLayout;
    navLayout->addWidget(m_navTips);
    navLayout->addSpacing(10);
    navLayout->addWidget(m_navView);
    navLayout->setSpacing(0);
    navLayout->setMargin(0);

    m_navWidget = new TranslucentFrame(this);
    m_navWidget->setLayout(navLayout);
    m_navWidget->setFixedSize(110 * 3 + 5, 500);
    m_navWidget->move(14, 0);
    m_navWidget->setVisible(false);

    setContent(m_settingsWidget);
    setTitle(tr("All Settings"));

    m_refershModuleActivableTimer->setSingleShot(true);
    m_refershModuleActivableTimer->setInterval(500);

#ifdef  DCC_KEEP_SETTINGS_LIVE
    connect(this, &SettingsWidget::back, this, &SettingsWidget::hideNavgation);
#endif
    connect(m_navView, &NavgationView::clicked, this, &SettingsWidget::toggleView);
    connect(m_navView, &NavgationView::entered, this, &SettingsWidget::onNavItemEntered);
    connect(m_navView, &NavgationView::entered, m_navModel, &NavgationModel::setCurrentItem);
    connect(m_navView, &NavgationView::clicked, this, &SettingsWidget::onNavItemClicked, Qt::QueuedConnection);
    connect(m_navgationBtn, &DImageButton::clicked, this, &SettingsWidget::toggleView);
    connect(m_resetBtn, &QPushButton::clicked, this, &SettingsWidget::resetAllSettings);
    connect(m_contentArea->verticalScrollBar(), &QScrollBar::valueChanged, m_refershModuleActivableTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(m_refershModuleActivableTimer, &QTimer::timeout, this, &SettingsWidget::refershModuleActivable);

    QMetaObject::invokeMethod(this, "loadModules", Qt::QueuedConnection);
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

    qDebug() << "setFrameAutoHide: " << inter << inter->name() << autoHide;

    emit requestAutohide(autoHide);
}

void SettingsWidget::pushWidget(ModuleInterface *const inter, ContentWidget *const w)
{
    Q_ASSERT(!m_moduleWidgets[inter].contains(w));

    m_moduleWidgets[inter].append(w);
    m_frame->pushWidget(w);
}

void SettingsWidget::mouseReleaseEvent(QMouseEvent *e)
{
    QWidget::mouseReleaseEvent(e);

    e->accept();

    if (m_navView->isVisible())
        toggleView();
    else if (e->button() == Qt::BackButton)
        emit back();
}

void SettingsWidget::loadModules()
{
    loadModule(new accounts::AccountsModule(this));
    loadModule(new display::DisplayModule(this));
    loadModule(new defapp::DefaultAppsModule(this));
    loadModule(new personalization::PersonalizationModule(this));
    loadModule(new network::NetworkModule(this));
    loadModule(new bluetooth::BluetoothModule(this));
    loadModule(new sound::SoundModule(this));
    loadModule(new DatetimeModule(this));
    loadModule(new power::PowerModule(this));
    loadModule(new mouse::MouseModule(this));
    loadModule(new KeyboardModule(this));
    loadModule(new wacom::WacomModule(this));
#ifndef DISABLE_SYS_UPDATE
    loadModule(new UpdateModule(this));
#endif
    loadModule(new SystemInfoModule(this));
}

void SettingsWidget::loadModule(ModuleInterface *const module)
{
    Q_ASSERT(!m_moduleInterfaces.contains(module));
    Q_ASSERT(!m_moduleWidgets.contains(module));

    m_moduleInterfaces.append(module);

    m_moduleWidgets.insert(module, QList<ContentWidget *>());
    m_navModel->appendAvailableItem(module->name());
    m_navModel->insertItem(module->name());

    ModuleInitThread *thrd = new ModuleInitThread(module, this);
    connect(thrd, &ModuleInitThread::moduleInitFinished, this, &SettingsWidget::onModuleInitFinished, Qt::QueuedConnection);
    connect(thrd, &ModuleInitThread::finished, thrd, &ModuleInitThread::deleteLater, Qt::QueuedConnection);

    if (module->name() == m_ensureVisibleModule)
        return thrd->start(QThread::HighestPriority);

    QTimer::singleShot(m_moduleLoadDelay, thrd, [=] { thrd->start(QThread::LowestPriority); });

    m_moduleLoadDelay += 50;
    m_moduleLoadDelay %= 340;
}

void SettingsWidget::onModuleInitFinished(ModuleInterface *const module)
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

    ModuleWidget *moduleWidget = module->moduleWidget();
    m_moduleActivable[module] = false;
    m_settingsLayout->insertWidget(index + 1, moduleWidget);

    connect(moduleWidget, &ModuleWidget::headerClicked, this, &SettingsWidget::toggleView);

    // load all modules finished
    if (m_moduleActivable.size() == m_moduleInterfaces.size())
    {
        m_refershModuleActivableTimer->start();

        // scroll to dest widget
        if (m_ensureVisiblePage.isEmpty())
            QTimer::singleShot(10, this, [=] { showModulePage(m_ensureVisibleModule, m_ensureVisiblePage); });
    }

    // show page
    if (m_ensureVisibleModule == module->name())
        QTimer::singleShot(10, this, [=] { showModulePage(m_ensureVisibleModule, m_ensureVisiblePage); });
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

void SettingsWidget::toggleView()
{
    if (sender() == m_navgationBtn)
        m_navWidget->move(m_navWidget->x(), 60);
    else
    {
        const int y = mapFromGlobal(QCursor::pos()).y() - 256;
        m_navWidget->move(m_navWidget->x(), std::min(std::max(60, y), height() - 500));
    }

    if (m_settingsWidget->isVisible())
    {
        m_navWidget->setVisible(true);
        m_settingsWidget->setVisible(false);
    } else {
        m_navWidget->setVisible(false);
        m_settingsWidget->setVisible(true);
    }
}

void SettingsWidget::showModulePage(const QString &moduleName, const QString &pageName)
{
    // hide navigation
    if (!m_settingsWidget->isVisible())
        toggleView();

    // test module is loaded
    bool founded = false;
    for (auto *module : m_moduleActivable.keys())
    {
        if (module->name() == moduleName)
        {
            founded = true;
            break;
        }
    }
    if (!founded)
    {
        m_ensureVisibleModule = moduleName;
        m_ensureVisiblePage = pageName;
        return;
    }

    if (pageName.isEmpty())
        return ensureModuleVisible(moduleName);

    for (auto *inter : m_moduleInterfaces)
        if (inter->name() == moduleName)
            inter->showPage(pageName);
}

void SettingsWidget::setModuleVisible(ModuleInterface * const inter, const bool visible)
{
    // get right position to insert
    const QString name = inter->name();

    QWidget *moduleWidget = inter->moduleWidget();
    Q_ASSERT(moduleWidget);
    moduleWidget->setVisible(visible);

    if (visible)
        m_navModel->insertItem(name);
    else
        m_navModel->removeItem(name);
}

void SettingsWidget::refershModuleActivable()
{
    stopScroll();

    QScroller *scroller = QScroller::scroller(m_contentArea);
    if (scroller->state() != QScroller::Inactive) {
        m_refershModuleActivableTimer->start();
        return;
    }

    const QRect containerRect = QRect(QPoint(), m_contentArea->size());

    for (ModuleInterface *module : m_moduleInterfaces)
    {
        if (!m_moduleActivable.contains(module))
            continue;

        const QWidget *w = module->moduleWidget();
        Q_ASSERT(w);
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

void SettingsWidget::onNavItemClicked(const QModelIndex &index)
{
    showModulePage(index.data().toString(), QString());
}

// TODO:
void SettingsWidget::onNavItemEntered(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    const QString name = index.data().toString();
    ModuleInterface *inter = nullptr;

    for (auto *it : m_moduleInterfaces)
    {
        if (it->name() == name)
        {
            inter = it;
            break;
        }
    }

    if (inter)
        m_navTips->setText(inter->moduleWidget()->title());
}

SettingsWidget::~SettingsWidget()
{
    for (auto v : m_moduleWidgets)
        qDeleteAll(v);
    qDeleteAll(m_moduleInterfaces);

    qDebug() << Q_FUNC_INFO << "I'm gone!!!";
}

void SettingsWidget::hideNavgation()
{
    if (!m_settingsWidget->isVisible())
        toggleView();
}
