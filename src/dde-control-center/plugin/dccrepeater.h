// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DCCREPEATER_H
#define DCCREPEATER_H

#include "dccobject.h"

#include <private/qqmlchangeset_p.h>

#include <QObject>
#include <QQmlComponent>
class QQmlChangeSet;

namespace dccV25 {
class DccRepeaterPrivate;

class DccRepeater : public DccObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged FINAL)
    Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged FINAL)
    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    QML_ELEMENT

public:
    explicit DccRepeater(QObject *parent = nullptr);
    ~DccRepeater() override;

    QVariant model() const;
    void setModel(const QVariant &newModel);

    QQmlComponent *delegate() const;
    void setDelegate(QQmlComponent *newDelegate);

    int count() const;

    Q_INVOKABLE void resetModel();
    Q_INVOKABLE DccObject *objectAt(int index) const;

Q_SIGNALS:
    void modelChanged();
    void delegateChanged();
    void countChanged();

    void objAdded(int index, QObject *obj);
    void objRemoved(int index, QObject *obj);

protected:
    void clear();
    void regenerate();
    void componentComplete() override;
    bool event(QEvent *event) override;

private Q_SLOTS:
    void createdItem(int index, QObject *item);
    void initItem(int index, QObject *item);
    void modelUpdated(const QQmlChangeSet &changeSet, bool reset);

private:
    QScopedPointer<DccRepeaterPrivate> d_ptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), DccRepeater)
};
} // namespace dccV25
#endif // DCCREPEATER_H
