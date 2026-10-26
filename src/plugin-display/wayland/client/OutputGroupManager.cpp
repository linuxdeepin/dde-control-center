// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "OutputGroupManager.h"

#include "WayQtLogging.h"

#include <wayland-client.h>

#include <QDebug>
#include <QLoggingCategory>

// The state event carries the mirror uuids as one flat byte array: every uuid
// is fixed-length and never contains NUL, so there are no separators. The
// compositor concatenates count uuids back to back (see treeland's
// OutputGroupInterfaceV1Private::sendState), so each entry is size / count
// bytes long.
static QStringList parseUuidArray(struct wl_array *array, uint32_t count)
{
    QStringList result;
    if (!array || count == 0 || array->size < static_cast<size_t>(count))
        return result;

    const size_t length = array->size / count;
    const char *cursor = static_cast<const char *>(array->data);
    result.reserve(static_cast<qsizetype>(count));
    for (uint32_t i = 0; i < count; ++i) {
        result.append(QString::fromUtf8(cursor, static_cast<qsizetype>(length)));
        cursor += length;
    }

    return result;
}

// ── WQt::OutputGroup ──────────────────────────────────────────────

WQt::OutputGroup::OutputGroup(::treeland_output_group_v1 *obj, QObject *parent)
    : QWaylandClientExtensionTemplate<WQt::OutputGroup>(1)
    , QtWayland::treeland_output_group_v1(obj)
{
    setParent(parent);
    qCDebug(DccWayQt) << "OutputGroup created";
}

WQt::OutputGroup::~OutputGroup()
{
    qCDebug(DccWayQt) << "OutputGroup destroyed:" << mName;
    if (isInitialized())
        QtWayland::treeland_output_group_v1::destroy();
}

QString WQt::OutputGroup::name() const
{
    return mName;
}

QString WQt::OutputGroup::sourceUuid() const
{
    return mSourceUuid;
}

QStringList WQt::OutputGroup::mirrorUuids() const
{
    return mMirrorUuids;
}

void WQt::OutputGroup::setSource(const QString &uuid)
{
    QtWayland::treeland_output_group_v1::set_source(uuid);
}

void WQt::OutputGroup::addOutput(const QString &uuid)
{
    QtWayland::treeland_output_group_v1::add_output(uuid);
}

void WQt::OutputGroup::removeOutput(const QString &uuid)
{
    QtWayland::treeland_output_group_v1::remove_output(uuid);
}

void WQt::OutputGroup::commit()
{
    QtWayland::treeland_output_group_v1::commit();
}

void WQt::OutputGroup::dissolve()
{
    QtWayland::treeland_output_group_v1::dissolve();
}

void WQt::OutputGroup::treeland_output_group_v1_name(const QString &name)
{
    mName = name;
    qCInfo(DccWayQt) << "OutputGroup name:" << mName;
    Q_EMIT nameChanged(mName);
}

void WQt::OutputGroup::treeland_output_group_v1_state(const QString &source_uuid, uint32_t count, wl_array *uuids)
{
    mSourceUuid = source_uuid;
    mMirrorUuids = parseUuidArray(uuids, count);
    qCInfo(DccWayQt) << "OutputGroup state:" << mName << "source:" << mSourceUuid << "mirrors:" << mMirrorUuids;
    Q_EMIT stateChanged(mSourceUuid, mMirrorUuids);
}

void WQt::OutputGroup::treeland_output_group_v1_done()
{
    if (!mInitialStateDone) {
        mInitialStateDone = true;
        Q_EMIT initialStateDone();
    }
}

void WQt::OutputGroup::treeland_output_group_v1_removed()
{
    Q_EMIT removed();
}

void WQt::OutputGroup::treeland_output_group_v1_operation_failed(uint32_t code, const QString &uuid)
{
    qCWarning(DccWayQt) << "OutputGroup operation failed:" << mName << "code:" << code << "uuid:" << uuid;
    Q_EMIT operationFailed(code, uuid);
}

// ── WQt::OutputGroupManager ───────────────────────────────────────

WQt::OutputGroup *WQt::OutputGroupManager::registerGroup(const QString &name, ::treeland_output_group_v1 *obj)
{
    if (!obj) {
        return nullptr;
    }

    auto *group = new WQt::OutputGroup(obj, this);
    setupGroupConnections(group);
    mGroups.insert(name, group);
    qCDebug(DccWayQt) << "registerGroup:" << name << group;
    return group;
}

void WQt::OutputGroupManager::setupGroupConnections(WQt::OutputGroup *group)
{
    connect(group, &WQt::OutputGroup::nameChanged, this, [this, group](const QString &name) {
        QString tempKey = QStringLiteral("__pending_%1").arg(reinterpret_cast<quintptr>(group));
        if (mGroups.contains(tempKey))
            mGroups.remove(tempKey);
        if (!mGroups.contains(name)) {
            mGroups.insert(name, group);
            Q_EMIT groupAdded(name);
        }
    });
    connect(group, &WQt::OutputGroup::stateChanged, this, [this, group](const QString &sourceUuid, const QStringList &mirrorUuids) {
        Q_EMIT groupStateChanged(group->name(), sourceUuid, mirrorUuids);
    });
    connect(group, &WQt::OutputGroup::operationFailed, this, [this, group](uint32_t code, const QString &uuid) {
        Q_EMIT groupOperationFailed(group->name(), code, uuid);
    });
    connect(group, &WQt::OutputGroup::removed, this, [this, group]() {
        QString tempKey = QStringLiteral("__pending_%1").arg(reinterpret_cast<quintptr>(group));
        mGroups.remove(tempKey);
        QString name = group->name();
        mGroups.remove(name);
        Q_EMIT groupRemoved(name);
    });
    connect(group, &QObject::destroyed, this, [this, group]() {
        for (auto it = mGroups.begin(); it != mGroups.end(); ++it) {
            if (it.value() == group) {
                mGroups.erase(it);
                break;
            }
        }
    });
}

WQt::OutputGroupManager::OutputGroupManager(QObject *parent)
    : QWaylandClientExtensionTemplate<WQt::OutputGroupManager>(1)
{
    setParent(parent);
}

WQt::OutputGroupManager::~OutputGroupManager()
{
    for (auto *group : mGroups) {
        disconnect(group, &QObject::destroyed, this, nullptr);
    }
    qDeleteAll(mGroups);

    if (isInitialized() && QtWayland::treeland_output_group_manager_v1::version() >= 1)
        QtWayland::treeland_output_group_manager_v1::destroy();
}

WQt::OutputGroup *WQt::OutputGroupManager::createGroup(const QString &sourceUuid)
{
    auto *obj = QtWayland::treeland_output_group_manager_v1::create_group(sourceUuid);
    auto *group = new WQt::OutputGroup(obj, this);
    setupGroupConnections(group);
    QString tempKey = QStringLiteral("__pending_%1").arg(reinterpret_cast<quintptr>(group));
    mGroups.insert(tempKey, group);
    qCDebug(DccWayQt) << "createGroup with source uuid:" << sourceUuid;
    return group;
}

WQt::OutputGroup *WQt::OutputGroupManager::getGroup(const QString &name)
{
    if (mGroups.contains(name))
        return mGroups.value(name);
    return getOrCreateGroup(name);
}

WQt::OutputGroup *WQt::OutputGroupManager::getOrCreateGroup(const QString &name)
{
    if (mGroups.contains(name))
        return mGroups.value(name);

    auto *obj = QtWayland::treeland_output_group_manager_v1::get_group(name);
    return registerGroup(name, obj);
}

QMap<QString, WQt::OutputGroup *> WQt::OutputGroupManager::groups() const
{
    return mGroups;
}

void WQt::OutputGroupManager::treeland_output_group_manager_v1_group_added(const QString &name)
{
    if (!mGroups.contains(name)) {
        getOrCreateGroup(name);
    }
    Q_EMIT groupAdded(name);
}

void WQt::OutputGroupManager::treeland_output_group_manager_v1_groups_done()
{
    Q_EMIT groupsDone();
}

void WQt::OutputGroupManager::treeland_output_group_manager_v1_group_removed(const QString &name)
{
    auto *group = mGroups.take(name);
    if (group) {
        group->deleteLater();
    }
    Q_EMIT groupRemoved(name);
}
