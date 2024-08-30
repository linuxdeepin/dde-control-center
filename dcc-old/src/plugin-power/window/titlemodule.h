//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef TITLEMODULE_H
#define TITLEMODULE_H
#include "interface/moduleobject.h"

class QWidget;

class TitleModule : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    TitleModule(const QString &name, const QString &title, QObject *parent = nullptr);
    virtual QWidget *page() override;
};

#endif // TITLEMODULE_H
