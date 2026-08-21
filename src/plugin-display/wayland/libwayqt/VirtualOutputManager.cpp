// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "VirtualOutputManager.h"

#include "WayQtLogging.h"

#include <wayland-client.h>

#include <QDebug>
#include <QLoggingCategory>

static QStringList parseStringArray(struct wl_array *array)
{
    QStringList result;
    if (!array || array->size == 0)
        return result;

    const char *data = static_cast<const char *>(array->data);
    const char *end = data + array->size;

    while (data < end) {
        const char *nul = static_cast<const char *>(memchr(data, '\0', end - data));
        if (!nul) {
            nul = end;
        }
        if (nul > data) {
            result.append(QString::fromUtf8(data, nul - data));
        }
        data = nul + 1;
    }

    return result;
}

// ── WQt::VirtualOutput ──────────────────────────────────────────────

WQt::VirtualOutput::VirtualOutput(::treeland_virtual_output_v1 *obj, QObject *parent)
    : QWaylandClientExtensionTemplate<WQt::VirtualOutput>(1)
    , QtWayland::treeland_virtual_output_v1(obj)
{
    setParent(parent);
    qCDebug(DccWayQt) << "VirtualOutput created";
}

WQt::VirtualOutput::~VirtualOutput()
{
    qCDebug(DccWayQt) << "VirtualOutput destroyed:" << mName;
}

QString WQt::VirtualOutput::name() const
{
    return mName;
}

QStringList WQt::VirtualOutput::outputs() const
{
    return mOutputs;
}

void WQt::VirtualOutput::treeland_virtual_output_v1_outputs(const QString &name, wl_array *outputs)
{
    mName = name;
    mOutputs = parseStringArray(outputs);
    qCInfo(DccWayQt) << "VirtualOutput outputs:" << mName << mOutputs;
    Q_EMIT outputsChanged(mName, mOutputs);
}

void WQt::VirtualOutput::treeland_virtual_output_v1_error(uint32_t code, const QString &message)
{
    qCWarning(DccWayQt) << "VirtualOutput error:" << mName << "code:" << code << message;
    Q_EMIT error(mName, code, message);
}

// ── WQt::VirtualOutputManager ───────────────────────────────────────

WQt::VirtualOutput *WQt::VirtualOutputManager::registerVirtualOutput(const QString &name, ::treeland_virtual_output_v1 *obj)
{
    if (!obj) {
        return nullptr;
    }

    auto *vo = new WQt::VirtualOutput(obj, this);
    connect(vo, &WQt::VirtualOutput::outputsChanged, this, &WQt::VirtualOutputManager::virtualOutputOutputsChanged);
    connect(vo, &WQt::VirtualOutput::error, this, &WQt::VirtualOutputManager::virtualOutputError);
    connect(vo, &QObject::destroyed, this, [this, name]() {
        mVirtualOutputs.remove(name);
        Q_EMIT virtualOutputList(mVirtualOutputs.keys());
    });
    mVirtualOutputs.insert(name, vo);
    qCDebug(DccWayQt) << "registerVirtualOutput:" << name << vo;
    Q_EMIT virtualOutputList(mVirtualOutputs.keys());
    return vo;
}

WQt::VirtualOutputManager::VirtualOutputManager(QObject *parent)
    : QWaylandClientExtensionTemplate<WQt::VirtualOutputManager>(1)
{
    setParent(parent);
}

WQt::VirtualOutputManager::~VirtualOutputManager()
{
    for (auto *vo : mVirtualOutputs) {
        disconnect(vo, &QObject::destroyed, this, nullptr);
    }
    qDeleteAll(mVirtualOutputs);
}

WQt::VirtualOutput *WQt::VirtualOutputManager::createVirtualOutput(const QString &name, const QStringList &outputs)
{
    QByteArray screenArray;
    for (const QString &str : outputs) {
        screenArray.append(str.toUtf8());
        screenArray.append('\0');
    }
    screenArray.append('\0');
    auto *obj = QtWayland::treeland_virtual_output_manager_v1::create_virtual_output(name, screenArray);
    return registerVirtualOutput(name, obj);
}

void WQt::VirtualOutputManager::getVirtualOutputList()
{
    QtWayland::treeland_virtual_output_manager_v1::get_virtual_output_list();
}

WQt::VirtualOutput *WQt::VirtualOutputManager::getOrCreateVirtualOutput(const QString &name)
{
    if (mVirtualOutputs.contains(name))
        return mVirtualOutputs.value(name);

    auto *obj = QtWayland::treeland_virtual_output_manager_v1::get_virtual_output(name);
    return registerVirtualOutput(name, obj);
}

WQt::VirtualOutput *WQt::VirtualOutputManager::takeVirtualOutput(const QString &name)
{
    return mVirtualOutputs.take(name);
}

void WQt::VirtualOutputManager::destroyVirtualOutput(const QString &name)
{
    auto *vo = mVirtualOutputs.take(name);
    qCDebug(DccWayQt) << "destroyVirtualOutput:" << name << "remaining:" << mVirtualOutputs.keys();
    if (vo) {
        vo->QtWayland::treeland_virtual_output_v1::destroy();
        vo->deleteLater();
    }
    Q_EMIT virtualOutputList(mVirtualOutputs.keys());
}

QMap<QString, WQt::VirtualOutput *> WQt::VirtualOutputManager::virtualOutputs() const
{
    return mVirtualOutputs;
}

void WQt::VirtualOutputManager::treeland_virtual_output_manager_v1_virtual_output_list(wl_array *names)
{
    QStringList nameList = parseStringArray(names);

    for (const auto &name : nameList) {
        getOrCreateVirtualOutput(name);
    }

    Q_EMIT virtualOutputList(nameList);
}
