// SPDX-FileCopyrightText: 2025 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "treelandworker.h"

#include <private/qguiapplication_p.h>
#include <private/qwaylandintegration_p.h>
#include <private/qwaylandwindow_p.h>
#include <qlogging.h>

#include <QGuiApplication>

using namespace DCC_MOUSE;

TreeLandWorker::TreeLandWorker(QObject *parent)
    : QObject(parent)
{
}

void TreeLandWorker::active()
{
#ifdef Enable_Treeland
    if (m_appearanceManager.isNull()) {
        m_appearanceManager.reset(new TreelandAppearanceManager(this));
        connect(m_appearanceManager.get(),
                &TreelandAppearanceManager::activeChanged,
                this,
                [this]() {
                    if (m_appearanceManager->isActive()) {
                    } else {
                        // clear();
                    }
                });
    }
#endif
}

void TreeLandWorker::setCursorSize(int size)
{
#ifdef Enable_Treeland
    if (m_appearanceManager) {
        m_appearanceManager->set_cursor_size(size, size);
    }
#endif
}

#ifdef Enable_Treeland
TreelandAppearanceManager::TreelandAppearanceManager(QObject *parent)
    : QWaylandClientExtensionTemplate<TreelandAppearanceManager>(1)
{
    QtWaylandClient::QWaylandIntegration *waylandIntegration =
            static_cast<QtWaylandClient::QWaylandIntegration *>(
                    QGuiApplicationPrivate::platformIntegration());
    if (!waylandIntegration) {
        qWarning() << "waylandIntegration is nullptr!!!";
        return;
    }
    m_waylandDisplay = waylandIntegration->display();
    if (!m_waylandDisplay) {
        qWarning() << "waylandDisplay is nullptr!!!";
        return;
    }

    addListener();
    setParent(parent);
}

TreelandAppearanceManager::~TreelandAppearanceManager()
{
    removeListener();
    if (isInitialized())
        QtWayland::treeland_appearance_manager_v1::destroy();
}

void TreelandAppearanceManager::addListener()
{
    if (!m_waylandDisplay) {
        qWarning() << "waylandDisplay is nullptr!, skip addListener";
        return;
    }
    m_waylandDisplay->addRegistryListener(&handleListenerGlobal, this);
}

void TreelandAppearanceManager::removeListener()
{
    if (!m_waylandDisplay) {
        qWarning() << "waylandDisplay is nullptr!, skip removeListener";
        return;
    }
    m_waylandDisplay->removeListener(&handleListenerGlobal, this);
}

void TreelandAppearanceManager::handleListenerGlobal(
        void *data, wl_registry *registry, uint32_t id, const QString &interface, uint32_t version)
{
    if (interface == treeland_appearance_manager_v1_interface.name) {
        TreelandAppearanceManager *integration = static_cast<TreelandAppearanceManager *>(data);
        if (!integration) {
            qWarning() << "integration is nullptr!!!";
            return;
        }

        integration->init(registry, id, version);
    }
}
#endif
