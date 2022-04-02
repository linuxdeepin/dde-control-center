
/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <andywang_cm@deepin.com>
 *
 * Maintainer: andywang <andywang_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "interface/namespace.h"
#include "interface/plugininterface.h"
#include "interface/moduleobject.h"

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
    Q_PLUGIN_METADATA(IID PluginInterface_iid FILE "KeyboardPlugin.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    virtual QString name() const override;
    virtual ModuleObject *module() override;
};

class KeyboardModule : public ModuleObject
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

class ShortCutSettingMenuModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit ShortCutSettingMenuModule(const QString &name, const QString &displayName = {}, QObject *parent = nullptr)
        : ModuleObject(name, displayName, parent) {}
    virtual QWidget *extraButton() override;
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
