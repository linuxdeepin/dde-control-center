//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DISPLAYMODULE_H
#define DISPLAYMODULE_H

#include "interface/namespace.h"
#include "interface/pagemodule.h"
#include "interface/plugininterface.h"
#include "window/recognizewidget.h"

class Resolution;
class QVBoxLayout;

namespace DCC_NAMESPACE {
class DisplayModel;
class Monitor;
class DisplayWorker;

class DisplayPlugin : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.Display" FILE "DisplayPlugin.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    virtual QString name() const override;
    virtual ModuleObject *module() override;
    virtual QString location() const override;
};

class DisplayModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit DisplayModule(QObject *parent = nullptr);
    ~DisplayModule();
    virtual QWidget *page() override;
    DisplayWorker *work() { return m_worker; }
    DisplayModel *model() { return m_model; }

protected:
    virtual void active() override;
    virtual void deactive() override;

private Q_SLOTS:
    void onRequestSetResolution(Monitor *monitor, const uint mode);
    void onRequestSetRotate(Monitor *monitor, const int rotate);
    void onRequestSetFillMode(Monitor *monitor, const QString fillMode);
    void showSingleScreenWidget();
    void showMultiScreenWidget();
    void pushScreenWidget();
    int showTimeoutDialog(Monitor *monitor, const bool isFillMode = false);
    void onSetFillMode(QString currFullMode = "");
    void showDisplayRecognize();
    void updateWinsize(QRect rect = QRect(0,0,0,0));
    void setPrimaryScreen(QScreen *screen);

private:
    DisplayModel  *m_model;
    DisplayWorker *m_worker;
    QWidget *m_displayWidget;
    QMap<QString, RecognizeWidget *> m_recognizeWidget;
    QScreen *m_primaryScreen;
};
}

#endif // DISPLAYMODULE_H
