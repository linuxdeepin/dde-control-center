#pragma once

#include "interface/moduleobject.h"
#include "interface/plugininterface.h"

DCC_USE_NAMESPACE

class Plugin : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PluginInterface_iid FILE "plugin-test.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    virtual QString name() const override;
    virtual ModuleObject *module() override;
    virtual QString follow() const override;
    virtual int location() const override;
};

class LabelModule : public ModuleObject
{
    Q_OBJECT
public:
    LabelModule(QObject *parent = nullptr) : ModuleObject(parent) {}
    LabelModule(const QString &name, const QString &displayName = {}, QObject *parent = nullptr) : ModuleObject(name, displayName, parent) {}
    virtual QWidget *page() override;

    void setText(const QString &text);
    inline QString text() const { return m_text; }

private:
    QString m_text;
};
