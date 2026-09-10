// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "dccbenchmark.h"
#include "dccpluginloader.h"

#include <QObject>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlIncubator>
#include <QQueue>
#include <memory>

namespace dccV25 {

class DccAsyncModuleLoader : public QObject
{
    Q_OBJECT
public:
    explicit DccAsyncModuleLoader(QQmlEngine *engine, QObject *parent = nullptr);
    ~DccAsyncModuleLoader() override;

    // Enqueue a plugin for async module + main QML loading.
    // If another plugin is currently being loaded, it is queued.
    void enqueue(DccPluginLoader *loader);

    // Cancel all pending work and clear the queue
    void cancel();
    bool isLoading() const { return m_active; }

private Q_SLOTS:
    // QQmlComponent::statusChanged handlers for the async compile phases
    void onModuleComponentStatus(QQmlComponent::Status status);
    void onMainComponentStatus(QQmlComponent::Status status);
    void startNext();

private:
    // Queued self-invocation of startNext(); safe to call from any callback
    void scheduleNext();

private:
    void doLoadModule();
    void doLoadMain();
    // Called from DccAsyncIncubator::statusChanged (may run synchronously
    // inside component->create() for simple QML files)
    void onModuleIncubated(QQmlIncubator::Status status);
    void onMainIncubated(QQmlIncubator::Status status);

    // Incubator subclass: engine calls statusChanged directly, no polling
    class DccAsyncIncubator;

    QQmlEngine *m_engine = nullptr;
    bool m_active = false;

    QQueue<DccPluginLoader *> m_queue;
    DccPluginLoader *m_current = nullptr;

    // Held alive during async phases; released only in startNext()/cancel()
    std::unique_ptr<QQmlComponent> m_moduleComponent;
    std::unique_ptr<DccAsyncIncubator> m_moduleIncubator;
    std::unique_ptr<QQmlComponent> m_mainComponent;
    std::unique_ptr<DccAsyncIncubator> m_mainIncubator;
    // Non-null = we own it; null = handed to the created object (onMainIncubated)
    QQmlContext *m_mainContext = nullptr;
};

} // namespace dccV25
