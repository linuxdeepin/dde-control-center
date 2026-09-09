// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "qwayland-treeland-output-uuid-head-unstable-v1.h"
#include "qwayland-treeland-output-uuid-unstable-v1.h"

#include <QHash>
#include <QObject>
#include <QString>
#include <QtWaylandClient/QWaylandClientExtension>

struct zwlr_output_head_v1;

namespace WQt {

// Wraps a treeland_output_uuid_v1 object created by the head manager.
// The uuid event arrives asynchronously after get_uuid() is called.
class TreelandOutputUuid : public QObject, public QtWayland::treeland_output_uuid_v1
{
    Q_OBJECT

public:
    TreelandOutputUuid(::treeland_output_uuid_v1 *obj, struct zwlr_output_head_v1 *head, QObject *parent = nullptr);
    ~TreelandOutputUuid() override;

    struct zwlr_output_head_v1 *head() const { return m_head; }

protected:
    void treeland_output_uuid_v1_uuid(const QString &uuid) override;

Q_SIGNALS:
    void uuidReceived(struct zwlr_output_head_v1 *head, const QString &uuid);

private:
    struct zwlr_output_head_v1 *m_head;
};

// Binds the treeland_output_uuid_head_manager_v1 global.
// Obtains treeland_output_uuid_v1 objects via get_uuid(zwlr_output_head_v1)
// and maintains head ↔ uuid mappings.
class TreelandOutputUuidHead : public QWaylandClientExtensionTemplate<TreelandOutputUuidHead>, public QtWayland::treeland_output_uuid_head_manager_v1
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)

public:
    TreelandOutputUuidHead(QObject *parent = nullptr);
    ~TreelandOutputUuidHead() override;

    // Request uuid for a head. The uuid arrives asynchronously via uuidChanged.
    void requestUuid(struct zwlr_output_head_v1 *head);

    // Returns the uuid for a head, or an empty string if not yet received.
    QString uuidForHead(struct zwlr_output_head_v1 *head) const;

    // Returns the head associated with a uuid, or nullptr if unknown.
    struct zwlr_output_head_v1 *headForUuid(const QString &uuid) const;

    // Removes a head from the mappings (e.g. when the head is finished).
    void removeHead(struct zwlr_output_head_v1 *head);

Q_SIGNALS:
    void activeChanged();
    void uuidChanged(struct zwlr_output_head_v1 *head, const QString &uuid);

private:
    void onUuidReceived(struct zwlr_output_head_v1 *head, const QString &uuid);

    QHash<struct zwlr_output_head_v1 *, TreelandOutputUuid *> m_uuidObjects;
    QHash<struct zwlr_output_head_v1 *, QString> m_headToUuid;
    QHash<QString, struct zwlr_output_head_v1 *> m_uuidToHead;
};

} // namespace WQt
