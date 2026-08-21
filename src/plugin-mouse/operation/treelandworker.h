//SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef TREELANDWORKER_H
#define TREELANDWORKER_H

#include <QObject>
#include <QtWaylandClient/QWaylandClientExtension>
#include <QWaylandClientExtension>
#include <private/qwaylanddisplay_p.h>

#ifdef Enable_Treeland
#include "wayland-treeland-personalization-manager-v1-client-protocol.h"
#include "qwayland-treeland-personalization-manager-v1.h"
#endif

namespace DCC_MOUSE {

class PersonalizationManager;
class TreelandCursorContext;

class TreeLandWorker : public QObject
{
    Q_OBJECT
public:
    explicit TreeLandWorker(QObject *parent);
    void active();
    void init();

    void setCursorSize(int size);
private:
#ifdef Enable_Treeland
    QScopedPointer<PersonalizationManager> m_personalizationManager;
    QScopedPointer<TreelandCursorContext> m_cursorContext;
#endif
};

#ifdef Enable_Treeland

class PersonalizationManager: public QWaylandClientExtensionTemplate<PersonalizationManager>,
                              public QtWayland::treeland_personalization_manager_v1
{
    Q_OBJECT
public:
    explicit PersonalizationManager(QObject *parent = nullptr);

private:
    void addListener();
    void removeListener();

    static void handleListenerGlobal(void *data, wl_registry *registry, uint32_t id, const QString &interface, uint32_t version);

private:
    QtWaylandClient::QWaylandDisplay *m_waylandDisplay = nullptr;
};


class TreelandCursorContext : public QWaylandClientExtensionTemplate<TreelandCursorContext>,
                                     public QtWayland::treeland_personalization_cursor_context_v1
{
    Q_OBJECT
public:
    explicit TreelandCursorContext(struct ::treeland_personalization_cursor_context_v1 *context);
};
#endif
}
#endif
