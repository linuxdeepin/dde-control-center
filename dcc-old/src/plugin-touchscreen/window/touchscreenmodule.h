//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef TOUCHSCREENMODULE_H
#define TOUCHSCREENMODULE_H

#include "interface/plugininterface.h"
#include "pagemodule.h"
#include "types/touchscreeninfolist_v2.h"

#include <QObject>
class TouchScreenModel;
class QPushButton;
class QComboBox;

namespace DCC_NAMESPACE {
class SettingsGroupModule;

class TouchScreenPlugin : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.TouchScreen" FILE "TouchScreenPlugin.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    virtual QString name() const override;
    virtual ModuleObject *module() override;
    virtual QString location() const override;
};


class TouchScreenModule : public PageModule
{
    Q_OBJECT
public:
    explicit TouchScreenModule(QObject *parent = nullptr);
    ~TouchScreenModule();
    TouchScreenModel *model() { return m_model; }

Q_SIGNALS:
    void onChanged();

public Q_SLOTS:
    void save();

private:
    void init();
    void resetItems();
private:
    TouchScreenModel  *m_model;
    SettingsGroupModule *m_settingGroup;
    QMap<QString, QComboBox *> m_itemMap;
    QStringList m_monitors;
    TouchscreenInfoList_V2 m_touchScreens;
};
}
#endif // TOUCHSCREENMODULE_H
