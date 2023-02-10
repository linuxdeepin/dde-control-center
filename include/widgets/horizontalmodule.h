//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HORIZONTALMODULE_H
#define HORIZONTALMODULE_H

#include "interface/moduleobject.h"
namespace DCC_NAMESPACE {
class HorizontalModulePrivate;
class HorizontalModule : public ModuleObject
{
    Q_OBJECT
public:
    enum StretchType {
        NoStretch = 0,
        LeftStretch = 1,
        RightStretch = 2,
        AllStretch = LeftStretch | RightStretch,
    };
    explicit HorizontalModule(const QString &name, const QString &displayName, QObject *parent = nullptr);
    ~HorizontalModule() override;

    void setStretchType(StretchType stretchType);
    void setSpacing(const int spacing);

    void appendChild(ModuleObject *const module) override;
    void insertChild(QList<ModuleObject *>::iterator before, ModuleObject *const module) override;
    void insertChild(const int index, ModuleObject *const module) override;
    void removeChild(ModuleObject *const module) override;
    void removeChild(const int index) override;

    void appendChild(ModuleObject *const module, int stretch, Qt::Alignment alignment = Qt::Alignment());
    void insertChild(QList<ModuleObject *>::iterator before, ModuleObject *const module, int stretch, Qt::Alignment alignment = Qt::Alignment());
    void insertChild(const int index, ModuleObject *const module, int stretch, Qt::Alignment alignment = Qt::Alignment());

    QWidget *page() override;
    inline DCC_MODULE_TYPE getClassID() const override { return HORIZONTAL; }

    DCC_DECLARE_PRIVATE(HorizontalModule)
};
}
#endif // HORIZONTALMODULE_H
