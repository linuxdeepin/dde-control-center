//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"
#include "interface/plugininterface.h"
#include "interface/hlistmodule.h"
#include "interface/pagemodule.h"

#include <QObject>

namespace DCC_NAMESPACE {
class KeyboardWidget;
class GeneralKBSettingWidget;
class KBLayoutSettingWidget;
class SystemLanguageWidget;
class SystemLanguageSettingWidget;
class ShortCutSettingWidget;
class CustomContent;

class KeyboardModel;
class ShortcutModel;
class KeyboardWorker;
class KeyboardLayoutWidget;
struct ShortcutInfo;
class CustomEdit;
class ShortcutContent;

class KeyboardPlugin : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.Keyboard" FILE "KeyboardPlugin.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    virtual QString name() const override;
    virtual ModuleObject *module() override;
    virtual QString location() const override;
};

class KeyboardModule : public HListModule
{
    Q_OBJECT

public:
    explicit KeyboardModule(QObject *parent = nullptr);
    ~KeyboardModule();
    KeyboardModel *model() { return m_model; }
    ShortcutModel *shortcutModel() { return m_shortcutModel; }
    KeyboardWorker *worker() { return m_work; }
protected:
    virtual void active() override;

private:
    KeyboardModel *m_model;
    ShortcutModel *m_shortcutModel;
    KeyboardWorker *m_work;
};

class GeneralSettingModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit GeneralSettingModule(KeyboardModel *model, KeyboardWorker *worker, QObject *parent = nullptr)
        : ModuleObject(parent), m_model(model), m_worker(worker){}
    virtual QWidget *page() override;

private:
    KeyboardModel *m_model;
    KeyboardWorker *m_worker;
};

class KBLayoutSettingModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit KBLayoutSettingModule(KeyboardModel *model, KeyboardWorker *worker, QObject *parent = nullptr)
      : ModuleObject(parent), m_model(model), m_worker(worker){}
    virtual QWidget *page() override;

public Q_SLOTS:
    void onPushKeyboard(const QStringList &kblist);
    void setCurrentLayout(const QString &value);

private:
    KeyboardModel *m_model;
    KeyboardWorker *m_worker;
};

#ifndef DCC_DISABLE_LANUGAGE
class SystemLanguageSettingModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit SystemLanguageSettingModule(KeyboardModel *model, KeyboardWorker *worker, QObject *parent = nullptr)
      : ModuleObject(parent), m_model(model), m_worker(worker){}
    virtual QWidget *page() override;

public Q_SLOTS:
    void onPushSystemLanguageSetting();
    void onAddLocale(const QModelIndex &index);

private:
    KeyboardModel *m_model;
    KeyboardWorker *m_worker;
};
#endif

class ShortCutSettingMenuModule : public PageModule
{
    Q_OBJECT
public:
    explicit ShortCutSettingMenuModule(const QString &name, const QString &displayName = {}, QObject *parent = nullptr)
        : PageModule(name, displayName, parent) {}
};

class ShortCutSettingModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit ShortCutSettingModule(KeyboardModel *model, KeyboardWorker *worker, ShortcutModel *shortcutModel, QObject *parent = nullptr)
      : ModuleObject(parent), m_model(model), m_worker(worker), m_shortcutModel(shortcutModel){}
    virtual QWidget *page() override;

public Q_SLOTS:
    void onPushCustomShortcut();
    void onPushConflict(ShortcutInfo *info, const QString &shortcut);
    void onShortcutEdit(ShortcutInfo *info);

private:
    KeyboardModel *m_model;
    KeyboardWorker *m_worker;
    ShortcutModel *m_shortcutModel;
};
}
