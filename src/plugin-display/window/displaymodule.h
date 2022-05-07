/*
 * Copyright (C) 2021 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     Tinalu Shao <shaotianlu@uniontech.com>
 *
 * Maintainer: Tinalu Shao <shaotianlu@uniontech.com>
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

#ifndef DISPLAYMODULE_H
#define DISPLAYMODULE_H

#include "interface/namespace.h"
#include "interface/moduleobject.h"
#include "interface/plugininterface.h"
#include "src/plugin-display/window/recognizewidget.h"

class Resolution;
class QVBoxLayout;

namespace DCC_NAMESPACE {
class DisplayModel;
class Monitor;
class DisplayWorker;

class DisplayPlugin : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PluginInterface_iid FILE "DisplayPlugin.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    virtual QString name() const override;
    virtual ModuleObject *module() override;
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
    void onRequestSetResolution(Monitor *monitor, const int mode);
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
