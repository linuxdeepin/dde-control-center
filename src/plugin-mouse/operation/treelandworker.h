// SPDX-FileCopyrightText: 2025 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef TREELANDWORKER_H
#define TREELANDWORKER_H

#include <QObject>
#include <QtWaylandClient/QWaylandClientExtension>
#include <QWaylandClientExtension>
#include <private/qwaylanddisplay_p.h>

#ifdef Enable_Treeland
#include "wayland-treeland-appearance-manager-unstable-v1-client-protocol.h"
#include "qwayland-treeland-appearance-manager-unstable-v1.h"
#endif

namespace DCC_MOUSE {

class TreelandAppearanceManager;

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
    QScopedPointer<TreelandAppearanceManager> m_appearanceManager;
#endif
};

#ifdef Enable_Treeland

class TreelandAppearanceManager: public QWaylandClientExtensionTemplate<TreelandAppearanceManager>,
                                  public QtWayland::treeland_appearance_manager_v1
{
    Q_OBJECT
public:
    explicit TreelandAppearanceManager(QObject *parent = nullptr);
    ~TreelandAppearanceManager() override;

private:
    void addListener();
    void removeListener();

    static void handleListenerGlobal(void *data, wl_registry *registry, uint32_t id, const QString &interface, uint32_t version);

private:
    QtWaylandClient::QWaylandDisplay *m_waylandDisplay = nullptr;
};
#endif
}
#endif
