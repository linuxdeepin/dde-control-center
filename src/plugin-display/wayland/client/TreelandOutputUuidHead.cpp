// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TreelandOutputUuidHead.h"

#include "WayQtLogging.h"

#include <QDebug>

#include "wayland-treeland-output-uuid-unstable-v1-client-protocol.h"
#include "wayland-treeland-output-uuid-head-unstable-v1-client-protocol.h"

// ── WQt::TreelandOutputUuid ───────────────────────────────────────

WQt::TreelandOutputUuid::TreelandOutputUuid(::treeland_output_uuid_v1 *obj, struct zwlr_output_head_v1 *head, QObject *parent)
    : QObject(parent)
    , QtWayland::treeland_output_uuid_v1(obj)
    , m_head(head)
{
}

WQt::TreelandOutputUuid::~TreelandOutputUuid()
{
    destroy();
}

void WQt::TreelandOutputUuid::treeland_output_uuid_v1_uuid(const QString &uuid)
{
    qCDebug(DccWayQt) << "TreelandOutputUuid::uuid received" << uuid << "for head" << m_head;
    Q_EMIT uuidReceived(m_head, uuid);
}

// ── WQt::TreelandOutputUuidHead ───────────────────────────────────

WQt::TreelandOutputUuidHead::TreelandOutputUuidHead(QObject *parent)
    : QWaylandClientExtensionTemplate<TreelandOutputUuidHead>(treeland_output_uuid_head_manager_v1_interface.version)
{
    setParent(parent);
    connect(this, &TreelandOutputUuidHead::activeChanged, this, &TreelandOutputUuidHead::onActiveChanged);
}

WQt::TreelandOutputUuidHead::~TreelandOutputUuidHead()
{
    if (isInitialized())
        QtWayland::treeland_output_uuid_head_manager_v1::destroy();
}

void WQt::TreelandOutputUuidHead::requestUuid(struct zwlr_output_head_v1 *head)
{
    if (!head)
        return;

    // Avoid requesting twice for the same head.
    if (m_uuidObjects.contains(head))
        return;

    // Extension not yet active: defer the request until activeChanged fires.
    if (!isActive()) {
        m_pendingHeads.insert(head);
        return;
    }

    m_pendingHeads.remove(head);

    auto *uuid = get_uuid(head);
    if (!uuid)
        return;

    auto *wrapper = new TreelandOutputUuid(uuid, head, this);
    m_uuidObjects.insert(head, wrapper);
    connect(wrapper, &TreelandOutputUuid::uuidReceived, this, &TreelandOutputUuidHead::onUuidReceived);
}

QString WQt::TreelandOutputUuidHead::uuidForHead(struct zwlr_output_head_v1 *head) const
{
    return m_headToUuid.value(head);
}

struct zwlr_output_head_v1 *WQt::TreelandOutputUuidHead::headForUuid(const QString &uuid) const
{
    return m_uuidToHead.value(uuid, nullptr);
}

void WQt::TreelandOutputUuidHead::removeHead(struct zwlr_output_head_v1 *head)
{
    m_pendingHeads.remove(head);

    QString uuid = m_headToUuid.take(head);
    if (!uuid.isEmpty())
        m_uuidToHead.remove(uuid);

    auto it = m_uuidObjects.find(head);
    if (it != m_uuidObjects.end()) {
        delete it.value();
        m_uuidObjects.erase(it);
    }
}

void WQt::TreelandOutputUuidHead::onActiveChanged()
{
    if (!isActive())
        return;

    // Retry all heads that were requested while the extension was inactive.
    auto pending = m_pendingHeads;
    m_pendingHeads.clear();
    for (auto *head : pending)
        requestUuid(head);
}

void WQt::TreelandOutputUuidHead::onUuidReceived(struct zwlr_output_head_v1 *head, const QString &uuid)
{
    QString oldUuid = m_headToUuid.value(head);
    if (!oldUuid.isEmpty())
        m_uuidToHead.remove(oldUuid);

    m_headToUuid.insert(head, uuid);
    m_uuidToHead.insert(uuid, head);

    qCDebug(DccWayQt) << "TreelandOutputUuidHead: head" << head << "→ uuid" << uuid;
    Q_EMIT uuidChanged(head, uuid);
}
