/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef UPDATEMODULE_H
#define UPDATEMODULE_H

#include "interfaces/moduleinterface.h"
#include "updatectrlwidget.h"
#include "updatesettings.h"
#include "mirrorswidget.h"
#include "updatemodel.h"
#include "updatework.h"

#include <QObject>

namespace dcc{
namespace update{

class UpdateView;

class UpdateModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit UpdateModule(FrameProxyInterface *frame, QObject *parent = 0);
    ~UpdateModule();

private:
    void initialize();
    void moduleActive();
    void moduleDeactive();
    void reset();
    void contentPopped(ContentWidget * const w);
    ModuleWidget* moduleWidget();
    const QString name() const;
    void showPage(const QString &pageName);

private Q_SLOTS:
    void onPushUpdate();
    void onPushMirrorsView();
    void onPushSettings();

private:
    UpdateWorker* m_work;
    UpdateModel* m_model;
    UpdateView* m_updateView;
    UpdateCtrlWidget* m_updatePage;
    UpdateSettings* m_settingsPage;
    MirrorsWidget* m_mirrorsWidget;
};

}
}
#endif // UPDATEMODULE_H
