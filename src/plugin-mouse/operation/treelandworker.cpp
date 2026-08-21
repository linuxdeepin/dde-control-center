// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
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
    if (m_personalizationManager.isNull()) {
        m_personalizationManager.reset(new PersonalizationManager(this));
        connect(m_personalizationManager.get(),
                &PersonalizationManager::activeChanged,
                this,
                [this]() {
                    if (m_personalizationManager->isActive()) {
                        init();
                    } else {
                        // clear();
                    }
                });
    }
#endif
}

void TreeLandWorker::init()
{
#ifdef Enable_Treeland
    if (m_cursorContext.isNull()) {
        m_cursorContext.reset(
                new TreelandCursorContext(m_personalizationManager->get_cursor_context()));
    }
#endif
}

void TreeLandWorker::setCursorSize(int size)
{
#ifdef Enable_Treeland
    if (m_cursorContext) {
        m_cursorContext->set_size(size);
        m_cursorContext->commit();
    }
#endif
}

#ifdef Enable_Treeland
PersonalizationManager::PersonalizationManager(QObject *parent)
    : QWaylandClientExtensionTemplate<PersonalizationManager>(1)
{
    if (QGuiApplication::platformName() == QLatin1String("wayland")) {
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
    }
    setParent(parent);
}

void PersonalizationManager::addListener()
{
    if (!m_waylandDisplay) {
        qWarning() << "waylandDisplay is nullptr!, skip addListener";
        return;
    }
    m_waylandDisplay->addRegistryListener(&handleListenerGlobal, this);
}

void PersonalizationManager::removeListener()
{
    if (!m_waylandDisplay) {
        qWarning() << "waylandDisplay is nullptr!, skip removeListener";
        return;
    }
    m_waylandDisplay->removeListener(&handleListenerGlobal, this);
}

void PersonalizationManager::handleListenerGlobal(
        void *data, wl_registry *registry, uint32_t id, const QString &interface, uint32_t version)
{
    if (interface == treeland_personalization_manager_v1_interface.name) {
        PersonalizationManager *integration = static_cast<PersonalizationManager *>(data);
        if (!integration) {
            qWarning() << "integration is nullptr!!!";
            return;
        }

        integration->init(registry, id, version);
    }
}

TreelandCursorContext::TreelandCursorContext(
        struct ::treeland_personalization_cursor_context_v1 *context)
    : QWaylandClientExtensionTemplate<TreelandCursorContext>(1)
    , QtWayland::treeland_personalization_cursor_context_v1(context)
{
}
#endif