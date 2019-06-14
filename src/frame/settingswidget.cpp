/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "settingswidget.h"
#include "frame.h"
#include "moduleinitthread.h"
#include "modules/modulewidget.h"
#include "navigationdelegate.h"
#include "widgets/backbutton.h"

#include "modules/accounts/accountsmodule.h"
#include "modules/bluetooth/bluetoothmodule.h"
#include "modules/datetime/datetimemodule.h"
#include "modules/defapp/defaultappsmodule.h"
#include "modules/keyboard/keyboardmodule.h"
#include "modules/power/powermodule.h"
#include "modules/sound/soundmodule.h"
#include "modules/update/updatemodule.h"
#include "modules/mouse/mousemodule.h"
#include "modules/wacom/wacommodule.h"
#include "modules/display/displaymodule.h"
#include "modules/personalization/personalizationmodule.h"
#include "modules/sync/syncmodule.h"
#include "modules/systeminfo/systeminfomodule.h"
#include "modules/network/networkmodule.h"

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

      m_moduleLoadDelay(0),
      m_frame(frame),
      m_visibleQueueCount(0),

      m_resetBtn(new QPushButton),
      m_settingsLayout(new QVBoxLayout),
      m_settingsWidget(new TranslucentFrame),

      m_refreshModuleActivableTimer(new QTimer(this))

{
    qDebug() << Q_FUNC_INFO << "I'm born!!!!";

#ifdef DISABLE_MAIN_PAGE
    m_backBtn->setVisible(false);
#endif

    m_resetBtn->setText(tr("Reset all settings"));

    m_settingsLayout->setSpacing(30);
    m_settingsLayout->setMargin(0);
    m_settingsLayout->addSpacing(10);
    m_settingsLayout->addSpacing(20);

    m_settingsWidget->setLayout(m_settingsLayout);

    setContent(m_settingsWidget);
    setTitle(tr("All Settings"));

    m_refreshModuleActivableTimer->setSingleShot(true);
    m_refreshModuleActivableTimer->setInterval(500);

    connect(m_resetBtn, &QPushButton::clicked, this, &SettingsWidget::resetAllSettings);
    connect(m_contentArea->verticalScrollBar(), &QScrollBar::valueChanged, m_refreshModuleActivableTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(m_refreshModuleActivableTimer, &QTimer::timeout, this, &SettingsWidget::refershModuleActivable, Qt::QueuedConnection);
    connect(this, &SettingsWidget::wheelValueChanged, this, &SettingsWidget::refreshNavigationbar);

    QMetaObject::invokeMethod(this, "loadModules", Qt::QueuedConnection);
}

void SettingsWidget::contentPopuped(ContentWidget *const w)
{
    QMap<ModuleInterface *, QList<ContentWidget *>>::iterator it =
        std::find_if(m_moduleWidgets.begin(), m_moduleWidgets.end(),
                     [=](const QList<ContentWidget *> &list) {
                         return !list.isEmpty() && list.contains(w);
                     });

    Q_ASSERT(it != m_moduleWidgets.end());

    it.key()->contentPopped(w);
    it.value().removeAll(w);
}

void SettingsWidget::setFrameAutoHide(ModuleInterface *const inter, const bool autoHide)
{
    Q_ASSERT(m_moduleInterfaces.contains(inter));

    qDebug() << "setFrameAutoHide: " << inter << inter->name() << autoHide;

    if (autoHide) {
        --m_visibleQueueCount;
    } else {
        ++m_visibleQueueCount;
    }

    if (m_visibleQueueCount == 0) {
        QTimer::singleShot(100, this, [=] {
            m_frame->setAutoHide(true);
        });
    } else {
        m_frame->setAutoHide(false);
    }
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

    if (e->button() == Qt::BackButton)
        Q_EMIT back();
}

void SettingsWidget::loadModules()
{
#ifndef DISABLE_ACCOUNT
    loadModule(new accounts::AccountsModule(this));
#endif

#ifndef DISABLE_CLOUD_SYNC
    loadModule(new cloudsync::SyncModule(this));
#endif

#ifndef DISABLE_DISPALY
    loadModule(new display::DisplayModule(this));
#endif

#ifndef DISABLE_DEFAULT_APPLICATIONS
    loadModule(new defapp::DefaultAppsModule(this));
#endif

#ifndef DISABLE_PERSONALIZATION
    loadModule(new personalization::PersonalizationModule(this));
#endif

#ifndef DISABLE_NETWORK
    loadModule(new network::NetworkModule(this));
#endif

#ifndef DISABLE_BLUETOOTH
    loadModule(new bluetooth::BluetoothModule(this));
#endif

#ifndef DISABLE_SOUND
    loadModule(new sound::SoundModule(this));
#endif

#ifndef DISABLE_DATETIME
    loadModule(new DatetimeModule(this));
#endif

#ifndef DISABLE_POWER
    loadModule(new power::PowerModule(this));
#endif

#ifndef DISABLE_MOUSE
    loadModule(new mouse::MouseModule(this));
#endif

#ifndef DISABLE_KAYBOARD
    loadModule(new KeyboardModule(this));
#endif

#ifndef DISABLE_WACOM
    loadModule(new wacom::WacomModule(this));
#endif

#ifndef DISABLE_SYS_UPDATE
    loadModule(new UpdateModule(this));
#endif

#ifndef DISABLE_SYSINFO
    loadModule(new SystemInfoModule(this));
#endif
}

void SettingsWidget::loadModule(ModuleInterface *const module)
{
    Q_ASSERT(!m_moduleInterfaces.contains(module));
    Q_ASSERT(!m_moduleWidgets.contains(module));

    m_moduleInterfaces.append(module);

    m_moduleWidgets.insert(module, QList<ContentWidget *>());

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

    // load all modules finished
    if (m_moduleActivable.size() == m_moduleInterfaces.size())
    {
        m_refreshModuleActivableTimer->start();

        // scroll to dest widget
        if (m_ensureVisiblePage.isEmpty())
            QTimer::singleShot(10, this, [=] { showModulePage(m_ensureVisibleModule, m_ensureVisiblePage, false); });
    }

    // show page
    if (m_ensureVisibleModule == module->name())
        QTimer::singleShot(10, this, [=] { showModulePage(m_ensureVisibleModule, m_ensureVisiblePage, false); });
}

void SettingsWidget::ensureModuleVisible(const QString &moduleName, bool animation)
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

    if (inter) {
        Q_EMIT currentModuleChanged(moduleName);
        scrollToWidget(inter->moduleWidget(), animation);
    }
}

void SettingsWidget::showModulePage(const QString &moduleName, const QString &pageName, bool animation)
{
    // stop the animation before playing.
    stopScroll();

    // whatever module/page found or not, refresh module active state is necessary.
    QTimer::singleShot(!animation ? 1 : m_scrollAni->duration(),
                       m_refreshModuleActivableTimer, static_cast<void (QTimer::*)()>(&QTimer::start));

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

    ensureModuleVisible(moduleName, animation);

    if (pageName.isEmpty())
        return;

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

    // notify navigation items dynamic change
    Q_EMIT moduleVisibleChanged(name, visible);
}

void SettingsWidget::refershModuleActivable()
{
    if (!isVisible())
        return;

    stopScroll();

    QScroller *scroller = QScroller::scroller(m_contentArea);
    if (scroller->state() != QScroller::Inactive) {
        m_refreshModuleActivableTimer->start();
        return;
    }

    refreshNavigationbar();
}

void SettingsWidget::refreshNavigationbar()
{
    const QRect containerRect = QRect(QPoint(), m_contentArea->size());
    const QString currentModuleName = m_frame->currentModuleName();

    ModuleInterface *firstActiveModule = nullptr;
    ModuleInterface *currentModule = nullptr;
    for (ModuleInterface *module : m_moduleInterfaces)
    {
        if (!m_moduleActivable.contains(module))
            continue;

        const QWidget *w = module->moduleWidget();
        Q_ASSERT(w);
        const QRect wRect(w->mapTo(m_contentArea, QPoint()), w->size());
        const bool activable = containerRect.intersects(wRect);

        if (!firstActiveModule && wRect.top() >= containerRect.top())
            firstActiveModule = module;

        if (currentModuleName == module->name())
            currentModule = module;

        if (m_moduleActivable[module] == activable)
            continue;

        m_moduleActivable[module] = activable;
        if (activable)
            module->moduleActive();
        else
            module->moduleDeactive();
    }

    if (currentModule && m_moduleActivable[currentModule])
        return;

    if (!firstActiveModule)
    {
        auto it = std::find_if(m_moduleActivable.begin(), m_moduleActivable.end(), [](bool b) -> bool {
            return b;
        });

        if (it != m_moduleActivable.end())
            firstActiveModule = it.key();
    }

    if (firstActiveModule)
        Q_EMIT currentModuleChanged(firstActiveModule->name());
}

void SettingsWidget::resetAllSettings()
{
    for (auto *inter : m_moduleInterfaces)
        inter->reset();
}

void SettingsWidget::onNavItemClicked(const QModelIndex &index)
{
    showModulePage(index.data().toString(), QString(), false);
}

SettingsWidget::~SettingsWidget()
{
    for (auto v : m_moduleWidgets)
    {
        for (auto w : v)
        {
            if (!w)
                continue;
            w->setVisible(false);
            w->setParent(nullptr);
            w->deleteLater();
        }
    }
    qDeleteAll(m_moduleInterfaces);

    qDebug() << Q_FUNC_INFO << "I'm gone!!!";
}
