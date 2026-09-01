// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "qwayland-treeland-virtual-output-manager-v1.h"

#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QtWaylandClient/QWaylandClientExtension>

namespace WQt {
class VirtualOutput;
class VirtualOutputManager;
} // namespace WQt

class WQt::VirtualOutput : public QWaylandClientExtensionTemplate<WQt::VirtualOutput>, public QtWayland::treeland_virtual_output_v1
{
    Q_OBJECT

    friend class VirtualOutputManager;

private:
    VirtualOutput(::treeland_virtual_output_v1 *obj, QObject *parent = nullptr);

public:
    ~VirtualOutput() override;

    QString name() const;
    QStringList outputs() const;

protected:
    void treeland_virtual_output_v1_outputs(const QString &name, wl_array *outputs) override;
    void treeland_virtual_output_v1_error(uint32_t code, const QString &message) override;

    QString mName;
    QStringList mOutputs;

Q_SIGNALS:
    void outputsChanged(const QString &name, const QStringList &outputs);
    void error(const QString &name, uint32_t code, const QString &message);
};

class WQt::VirtualOutputManager : public QWaylandClientExtensionTemplate<WQt::VirtualOutputManager>, public QtWayland::treeland_virtual_output_manager_v1
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)

public:
    VirtualOutputManager(QObject *parent = nullptr);
    ~VirtualOutputManager() override;

    WQt::VirtualOutput *createVirtualOutput(const QString &name, const QStringList &outputs);
    void getVirtualOutputList();
    WQt::VirtualOutput *takeVirtualOutput(const QString &name);
    void destroyVirtualOutput(const QString &name);
    QMap<QString, WQt::VirtualOutput *> virtualOutputs() const;

protected:
    void treeland_virtual_output_manager_v1_virtual_output_list(wl_array *names) override;

private:
    WQt::VirtualOutput *getOrCreateVirtualOutput(const QString &name);
    WQt::VirtualOutput *registerVirtualOutput(const QString &name, ::treeland_virtual_output_v1 *obj);

    QMap<QString, WQt::VirtualOutput *> mVirtualOutputs;

Q_SIGNALS:
    void virtualOutputList(const QStringList &names);
    void virtualOutputOutputsChanged(const QString &name, const QStringList &outputs);
    void virtualOutputError(const QString &name, uint32_t code, const QString &message);
};
