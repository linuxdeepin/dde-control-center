#pragma once

#include "interface/moduleobject.h"
#include "interface/plugininterface.h"
#include <qobject.h>
#include <qobjectdefs.h>
#include <qpushbutton.h>
#include <qwidget.h>

DCC_USE_NAMESPACE

class Plugin : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PluginInterface_iid FILE "plugin-test.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    virtual QString name() const override;
    virtual ModuleObject *module() override;
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

class ButtonModule : public ModuleObject
{
    Q_OBJECT
public:
    ButtonModule(QObject *parent = nullptr) : ModuleObject(parent) {}
    ButtonModule(const QString &name, const QString &displayName = {}, QObject *parent =nullptr) : ModuleObject(name, displayName, parent) {}
    virtual QWidget *page() override;
    virtual QWidget *extraButton() override;

    void setText(const QString &text);
    inline QString text() const { return m_text; }

Q_SIGNALS:
    void onButtonClicked();

private:
    QString m_text;
};
