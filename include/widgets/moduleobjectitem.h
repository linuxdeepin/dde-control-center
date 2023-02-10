//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MODULEOBJECTITEM_H
#define MODULEOBJECTITEM_H

#include "interface/moduleobject.h"

#include <DStyle>
#include <QVariant>

namespace DCC_NAMESPACE {
class ModuleObjectItemPrivate;
class ModuleObjectItem : public ModuleObject
{
    Q_OBJECT
public:
    explicit ModuleObjectItem(const QString &name, const QString &displayName, QObject *parent = nullptr);
    ~ModuleObjectItem() override;

    void setRightIcon(DTK_WIDGET_NAMESPACE::DStyle::StandardPixmap st, int index = -1);
    void setRightIcon(const QString &icon, int index = -1);
    void setRightIcon(const QIcon &icon, int index = -1);
    void setRightText(const QString &text, int index = -1);
    DTK_WIDGET_NAMESPACE::DViewItemAction *getRightItem(int index = -1);
    void update();

    virtual void setData(int role, const QVariant &value);
    virtual QVariant data(int role) const;
    inline DCC_MODULE_TYPE getClassID() const override { return ITEM; }

Q_SIGNALS:
    void clicked();

    DCC_DECLARE_PRIVATE(ModuleObjectItem)
};
}
#endif // MODULEOBJECTITEM_H
