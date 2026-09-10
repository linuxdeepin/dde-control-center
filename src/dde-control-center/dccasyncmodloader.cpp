// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dccasyncmodloader.h"

#include "dccobject.h"
#include "dccpluginloader.h"

#include <QLoggingCategory>
#include <QQmlContext>

namespace dccV25 {

Q_LOGGING_CATEGORY(dccAsyncLog, "dde.dcc.asyncmodloader")

// Incubator subclass: engine calls statusChanged directly, no polling
// (the callback may run synchronously inside component->create() for simple
// QML files, so the callback must NOT reset the incubator).
class DccAsyncModuleLoader::DccAsyncIncubator : public QQmlIncubator
{
public:
    enum Phase { ModulePhase, MainPhase };

    DccAsyncIncubator(DccAsyncModuleLoader *owner, Phase phase)
        : QQmlIncubator(Asynchronous)
        , m_owner(owner)
        , m_phase(phase)
    {
    }

protected:
    void statusChanged(Status status) override
    {
        if (m_phase == ModulePhase) {
            m_owner->onModuleIncubated(status);
        } else {
            m_owner->onMainIncubated(status);
        }
    }

private:
    DccAsyncModuleLoader *m_owner;
    Phase m_phase;
};

DccAsyncModuleLoader::DccAsyncModuleLoader(QQmlEngine *engine, QObject *parent)
    : QObject(parent)
    , m_engine(engine)
{
}

DccAsyncModuleLoader::~DccAsyncModuleLoader()
{
    cancel();
}

void DccAsyncModuleLoader::enqueue(DccPluginLoader *loader)
{
    if (!loader || !m_engine) {
        return;
    }

    m_queue.enqueue(loader);
    if (m_active) {
        qCDebug(dccAsyncLog) << "Queuing" << loader->name() << "(queue size=" << m_queue.size() << ")";
        return;
    }

    m_active = true;
    m_current = loader;
    startNext();
}

void DccAsyncModuleLoader::cancel()
{
    m_queue.clear();
    m_active = false;
    m_current = nullptr;
    // Abort any in-flight incubation before releasing the objects
    if (m_moduleIncubator) {
        m_moduleIncubator->clear();
    }
    if (m_mainIncubator) {
        m_mainIncubator->clear();
    }
    m_moduleComponent.reset();
    m_moduleIncubator.reset();
    m_mainComponent.reset();
    m_mainIncubator.reset();
    // Non-null = we own it; null = already handed to the created object
    delete m_mainContext;
    m_mainContext = nullptr;
}

void DccAsyncModuleLoader::doLoadModule()
{
    if (!m_current || !m_engine) {
        return;
    }
    if (!(m_current->type() & DccPluginLoader::T_HasModule)) {
        // Same as sync loadModule(): a missing module QML is not an error
        m_current->setLog("module qml not exists");
        m_current->transitionStatus(DccPluginLoader::ModuleEnd);
        scheduleNext();
        return;
    }
    const QString name = m_current->name();

    m_current->transitionStatus(DccPluginLoader::ModuleLoad);

    DCC_BENCHMARK(name, "loading-module-QML");
    m_moduleComponent = std::make_unique<QQmlComponent>(m_engine);
    // Async compilation: QML parsing doesn't block the main loop
    connect(m_moduleComponent.get(), &QQmlComponent::statusChanged, this, &DccAsyncModuleLoader::onModuleComponentStatus, Qt::QueuedConnection);
    switch (m_current->version()) {
    case DccPluginLoader::T_V1_0: {
        const QString qmlPath = m_current->path() + "/" + m_current->name() + ".qml";
        m_moduleComponent->loadUrl(qmlPath, QQmlComponent::Asynchronous);
        m_current->setLog("create module " + qmlPath);
        DCC_BENCHMARK(m_current->name(), "module-qml-load");
    } break;
    case DccPluginLoader::T_V1_1:
    default: {
        QString typeName = m_current->name();
        typeName[0] = typeName[0].toUpper();
        m_current->setLog("create module " + typeName);
        DCC_BENCHMARK(m_current->name(), "module-qml-load");
        m_moduleComponent->loadFromModule(m_current->name(), typeName, QQmlComponent::Asynchronous);
    } break;
    }
}

void DccAsyncModuleLoader::onModuleComponentStatus(QQmlComponent::Status status)
{
    if (!m_current) {
        return; // callback delivered after cancel()/startNext() cleared the current item
    }
    const QString name = m_current->name();
    switch (status) {
    case QQmlComponent::Error: {
        m_current->setLog("component create module object error:" + m_moduleComponent->errorString());
        m_current->transitionStatus(DccPluginLoader::ModuleErr | DccPluginLoader::ModuleEnd);
        scheduleNext();
    } break;
    case QQmlComponent::Ready: {
        m_current->transitionStatus(DccPluginLoader::ModuleCreate);
        DCC_BENCHMARK(name, "module-object-create");

        // Start async incubation; completion delivered via statusChanged override
        m_moduleIncubator = std::make_unique<DccAsyncIncubator>(this, DccAsyncIncubator::ModulePhase);
        m_moduleComponent->create(*m_moduleIncubator);
    } break;
    default:
        break;
    }
}

void DccAsyncModuleLoader::onModuleIncubated(QQmlIncubator::Status status)
{
    if (!m_current || !m_moduleIncubator) {
        return;
    }

    const QString name = m_current->name();
    switch (status) {
    case QQmlIncubator::Ready: {
        QObject *rawObj = m_moduleIncubator->object();
        if (!rawObj) {
            const QString err = m_moduleIncubator->errors().isEmpty() ? QString() : m_moduleIncubator->errors().first().description();
            m_current->setLog("component create module object is null:" + err);
            m_current->transitionStatus(DccPluginLoader::ModuleErr | DccPluginLoader::ModuleEnd);
            scheduleNext();
            return;
        }

        DccObject *module = qobject_cast<DccObject *>(rawObj);
        DCC_BENCHMARK(name, "loading-module-QML");

        m_current->setModule(module);
        m_current->transitionStatus(DccPluginLoader::ModuleEnd);
        scheduleNext();
    } break;
    case QQmlIncubator::Error: {
        const QString err = m_moduleIncubator->errors().isEmpty() ? QString() : m_moduleIncubator->errors().first().description();
        m_current->setLog("component create module object error:" + err);
        m_current->transitionStatus(DccPluginLoader::ModuleErr | DccPluginLoader::ModuleEnd);
        scheduleNext();
    } break;
    default:
        break;
    }
}

void DccAsyncModuleLoader::doLoadMain()
{
    if (!m_current || !m_engine) {
        return;
    }

    const QString name = m_current->name();
    const QString path = m_current->path();
    const auto type = m_current->type();
    const auto ver = m_current->version();

    if (!(type & DccPluginLoader::T_HasMain)) {
        m_current->setLog("main qml not exists");
        m_current->transitionStatus(DccPluginLoader::MainObjErr | DccPluginLoader::MainObjEnd);
        scheduleNext();
        return;
    }

    m_current->transitionStatus(DccPluginLoader::MainObjLoad);

    // Async compilation for main QML
    m_mainComponent = std::make_unique<QQmlComponent>(m_engine);
    connect(m_mainComponent.get(), &QQmlComponent::statusChanged, this, &DccAsyncModuleLoader::onMainComponentStatus, Qt::QueuedConnection);
    if (ver == DccPluginLoader::T_V1_0) {
        const QString qmlPath = path + "/" + ((type & DccPluginLoader::T_ShortMain) ? "main.qml" : name + "Main.qml");
        m_mainComponent->loadUrl(qmlPath, QQmlComponent::Asynchronous);
    } else {
        QString typeName = name + "Main";
        typeName[0] = typeName[0].toUpper();
        m_mainComponent->loadFromModule(name, typeName, QQmlComponent::Asynchronous);
    }
}

void DccAsyncModuleLoader::onMainComponentStatus(QQmlComponent::Status status)
{
    if (!m_current) {
        return; // callback delivered after cancel()/startNext() cleared the current item
    }
    switch (status) {
    case QQmlComponent::Error: {
        const QString name = m_current->name();
        m_current->setLog(" component create main object error:" + m_mainComponent->errorString());
        m_current->transitionStatus(DccPluginLoader::MainObjErr | DccPluginLoader::MainObjEnd);
        scheduleNext();
    } break;
    case QQmlComponent::Ready: {
        m_current->transitionStatus(DccPluginLoader::MainObjCreate);
        DCC_BENCHMARK(m_current->name(), "mainobj-sub-step:qml-create()");

        // Context must outlive async creation; parented to the created
        // object in onMainIncubated
        m_mainContext = new QQmlContext(m_engine);
        m_mainContext->setContextProperties({ { "dccData", QVariant::fromValue(m_current->data()) }, { "dccModule", QVariant::fromValue(m_current->module()) } });

        // Start async incubation; completion delivered via statusChanged override
        m_mainIncubator = std::make_unique<DccAsyncIncubator>(this, DccAsyncIncubator::MainPhase);
        m_mainComponent->create(*m_mainIncubator, m_mainContext);
    } break;
    default:
        break;
    }
}

void DccAsyncModuleLoader::onMainIncubated(QQmlIncubator::Status status)
{
    if (!m_current || !m_mainIncubator) {
        return;
    }

    const QString name = m_current->name();
    switch (status) {
    case QQmlIncubator::Ready: {
        QObject *rawObj = m_mainIncubator->object();
        if (!rawObj) {
            const QString err = m_mainIncubator->errors().isEmpty() ? QString() : m_mainIncubator->errors().first().description();
            m_current->setLog(" component create main object is null:" + err);
            m_current->transitionStatus(DccPluginLoader::MainObjErr | DccPluginLoader::MainObjEnd);
            scheduleNext();
            // We still own the context; nothing adopted it
            delete m_mainContext;
            m_mainContext = nullptr;
            return;
        }
        DccObject *mainObj = qobject_cast<DccObject *>(rawObj);
        DCC_BENCHMARK(name, "loading-main-QML");
        qCDebug(dccAsyncLog) << name << "main object ready";

        m_current->setMainObj(mainObj);
        m_mainContext->setParent(mainObj);
        m_mainContext = nullptr;
        m_current->transitionStatus(DccPluginLoader::MainObjEnd);

        scheduleNext();
    } break;
    case QQmlIncubator::Error: {
        const QString err = m_mainIncubator->errors().isEmpty() ? QString() : m_mainIncubator->errors().first().description();
        m_current->setLog(" component create main object error:" + err);
        m_current->transitionStatus(DccPluginLoader::MainObjErr | DccPluginLoader::MainObjEnd);
        // We still own the context; nothing adopted it
        delete m_mainContext;
        m_mainContext = nullptr;
        scheduleNext();
    } break;
    default:
        break;
    }
}

void DccAsyncModuleLoader::scheduleNext()
{
    QMetaObject::invokeMethod(this, "startNext", Qt::QueuedConnection);
}

void DccAsyncModuleLoader::startNext()
{
    // Always invoked via queued connection, so releasing here is safe
    m_moduleComponent.reset();
    m_moduleIncubator.reset();
    m_mainComponent.reset();
    m_mainIncubator.reset();
    // Safety net: non-null here means the context was never adopted
    delete m_mainContext;
    m_mainContext = nullptr;
    m_current = nullptr;
    if (m_queue.isEmpty()) {
        m_active = false;
        return;
    }
    m_current = m_queue.dequeue();
    DccPluginLoader::StatusFlags status = m_current->status();
    if ((status & (DccPluginLoader::MetaDataEnd | DccPluginLoader::ModuleLoad)) == DccPluginLoader::MetaDataEnd) {
        doLoadModule();
    } else if ((status & (DccPluginLoader::DataEnd | DccPluginLoader::MainObjLoad)) == DccPluginLoader::DataEnd) {
        doLoadMain();
    } else {
        scheduleNext();
    }
}

} // namespace dccV25
