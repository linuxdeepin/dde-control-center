//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/moduleobject.h"
#include "interface/plugininterface.h"


class Test2Plugin : public DCC_NAMESPACE::PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.test2" FILE "plugin-test.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    virtual QString name() const override;
    virtual DCC_NAMESPACE::ModuleObject *module() override;
    virtual QString follow() const override;
    virtual QString location() const override;
};

class LabelModule : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    LabelModule(QObject *parent = nullptr) : DCC_NAMESPACE::ModuleObject(parent) {}
    LabelModule(const QString &name, const QString &displayName = {}, QObject *parent = nullptr) : ModuleObject(name, displayName, parent) {}
    virtual QWidget *page() override;

    void setText(const QString &text);
    inline QString text() const { return m_text; }

private:
    QString m_text;
};
