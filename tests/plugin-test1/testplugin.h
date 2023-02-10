//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/moduleobject.h"
#include "interface/plugininterface.h"
#include "interface/hlistmodule.h"
#include "interface/vlistmodule.h"

class Test1Plugin : public DCC_NAMESPACE::PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.Plugin_test" FILE "plugin-test.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    virtual QString name() const override;
    virtual DCC_NAMESPACE::ModuleObject *module() override;
};

class Test1ModuleObject : public DCC_NAMESPACE::HListModule
{
    Q_OBJECT
public:
    explicit Test1ModuleObject();

    QWidget *initButton();
    void addTestModule(ModuleObject *parent);
};

class LabelModule : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    LabelModule(QObject *parent = nullptr)
        : ModuleObject(parent) { }
    LabelModule(const QString &name, const QString &displayName = {}, QObject *parent = nullptr)
        : ModuleObject(name, displayName, parent) { }
    virtual QWidget *page() override;

    void setText(const QString &text);
    inline QString text() const { return m_text; }

private:
    QString m_text;
};

class ButtonModule : public DCC_NAMESPACE::VListModule
{
    Q_OBJECT
public:
    ButtonModule(QObject *parent = nullptr)
        : VListModule(parent) { }
    ButtonModule(const QString &name, const QString &displayName = {}, QObject *parent = nullptr)
        : VListModule(name, displayName, parent) { }
    virtual QWidget *page() override;

    void setText(const QString &text);
    inline QString text() const { return m_text; }

Q_SIGNALS:
    void onButtonClicked();

private:
    QString m_text;
};
