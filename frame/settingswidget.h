/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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

#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include "contentwidget.h"
#include "moduleinterface.h"
#include "translucentframe.h"
#include "navigationview.h"
#include "navigationmodel.h"

#include <QMap>
#include <QVBoxLayout>
#include <QPushButton>

using namespace dcc;
using namespace dcc::widgets;

class Frame;
class SettingsWidget : public ContentWidget, public FrameProxyInterface
{
    Q_OBJECT

public:
    explicit SettingsWidget(Frame *frame);
    ~SettingsWidget();

public slots:
    void contentPopuped(ContentWidget * const w);
    void showModulePage(const QString &moduleName, const QString &pageName);

    void setModuleVisible(ModuleInterface * const inter, const bool visible);
    void setFrameAutoHide(ModuleInterface * const inter, const bool autoHide);
    void pushWidget(ModuleInterface * const inter, ContentWidget * const w);

signals:
    void moduleVisibleChanged(const QString &module, bool visible);

protected:
    void mouseReleaseEvent(QMouseEvent *e);

private slots:
    void loadModules();
    void loadModule(ModuleInterface * const module);
    void onModuleInitFinished(ModuleInterface * const inter);
    void ensureModuleVisible(const QString &moduleName);
    void refershModuleActivable();
    void resetAllSettings();
    void onNavItemClicked(const QModelIndex &index);
    void onNavItemEntered(const QModelIndex &index);

private:
    int m_moduleLoadDelay;

    Frame *m_frame;

    QPushButton *m_resetBtn;
    QVBoxLayout *m_settingsLayout;
    TranslucentFrame *m_settingsWidget;

    QList<ModuleInterface *> m_moduleInterfaces;
    QMap<ModuleInterface *, bool> m_moduleActivable;
    QMap<ModuleInterface *, QList<ContentWidget *>> m_moduleWidgets;

    QTimer *m_refershModuleActivableTimer;

    QString m_ensureVisibleModule;
    QString m_ensureVisiblePage;
};

#endif // SETTINGSWIDGET_H
