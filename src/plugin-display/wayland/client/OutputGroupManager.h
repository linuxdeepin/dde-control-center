// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "qwayland-treeland-output-group-manager-unstable-v1.h"

#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QtWaylandClient/QWaylandClientExtension>

namespace WQt {
class OutputGroup;
class OutputGroupManager;
} // namespace WQt

class WQt::OutputGroup : public QWaylandClientExtensionTemplate<WQt::OutputGroup>, public QtWayland::treeland_output_group_v1
{
    Q_OBJECT

    friend class OutputGroupManager;

private:
    OutputGroup(::treeland_output_group_v1 *obj, QObject *parent = nullptr);

public:
    ~OutputGroup() override;

    QString name() const;
    QString sourceUuid() const;
    QStringList mirrorUuids() const;

    void setSource(const QString &uuid);
    void addOutput(const QString &uuid);
    void removeOutput(const QString &uuid);
    void commit();
    void dissolve();

protected:
    void treeland_output_group_v1_name(const QString &name) override;
    void treeland_output_group_v1_state(const QString &source_uuid, uint32_t count, wl_array *uuids) override;
    void treeland_output_group_v1_done() override;
    void treeland_output_group_v1_removed() override;
    void treeland_output_group_v1_operation_failed(uint32_t code, const QString &uuid) override;

    QString mName;
    QString mSourceUuid;
    QStringList mMirrorUuids;
    bool mInitialStateDone = false;

Q_SIGNALS:
    void nameChanged(const QString &name);
    void stateChanged(const QString &sourceUuid, const QStringList &mirrorUuids);
    void initialStateDone();
    void removed();
    void operationFailed(uint32_t code, const QString &uuid);
};

class WQt::OutputGroupManager : public QWaylandClientExtensionTemplate<WQt::OutputGroupManager>, public QtWayland::treeland_output_group_manager_v1
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)

public:
    OutputGroupManager(QObject *parent = nullptr);
    ~OutputGroupManager() override;

    WQt::OutputGroup *createGroup(const QString &sourceUuid);
    WQt::OutputGroup *getGroup(const QString &name);
    QMap<QString, WQt::OutputGroup *> groups() const;

protected:
    void treeland_output_group_manager_v1_group_added(const QString &name) override;
    void treeland_output_group_manager_v1_groups_done() override;
    void treeland_output_group_manager_v1_group_removed(const QString &name) override;

private:
    WQt::OutputGroup *getOrCreateGroup(const QString &name);
    WQt::OutputGroup *registerGroup(const QString &name, ::treeland_output_group_v1 *obj);
    void setupGroupConnections(WQt::OutputGroup *group);

    QMap<QString, WQt::OutputGroup *> mGroups;

Q_SIGNALS:
    void groupAdded(const QString &name);
    void groupsDone();
    void groupRemoved(const QString &name);
    void groupStateChanged(const QString &name, const QString &sourceUuid, const QStringList &mirrorUuids);
    void groupOperationFailed(const QString &name, uint32_t code, const QString &uuid);
};
