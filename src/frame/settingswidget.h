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

#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include "widgets/contentwidget.h"
#include "interfaces/moduleinterface.h"
#include "widgets/translucentframe.h"
#include "navigationview.h"
#include "navigationmodel.h"

#include <QMap>
#include <QVBoxLayout>
#include <QPushButton>
#include <QAtomicInt>

using namespace dcc;
using namespace dcc::widgets;

class Frame;
class SettingsWidget : public ContentWidget, public FrameProxyInterface
{
    Q_OBJECT

public:
    explicit SettingsWidget(Frame *frame);
    ~SettingsWidget();

public Q_SLOTS:
    void contentPopuped(ContentWidget * const w);
    void showModulePage(const QString &moduleName, const QString &pageName, bool animation);

    void setModuleVisible(ModuleInterface * const inter, const bool visible);
    void setFrameAutoHide(ModuleInterface * const inter, const bool autoHide);
    void pushWidget(ModuleInterface * const inter, ContentWidget * const w);

Q_SIGNALS:
    void moduleVisibleChanged(const QString &module, bool visible);
    void currentModuleChanged(const QString &module);

protected:
    void mouseReleaseEvent(QMouseEvent *e);

private Q_SLOTS:
    void loadModules();
    void loadModule(ModuleInterface * const module);
    void onModuleInitFinished(ModuleInterface * const inter);
    void ensureModuleVisible(const QString &moduleName, bool animation);
    void refershModuleActivable();
    void refreshNavigationbar();
    void resetAllSettings();
    void onNavItemClicked(const QModelIndex &index);

private:
    int m_moduleLoadDelay;

    Frame *m_frame;

    QAtomicInt m_visibleQueueCount;

    QPushButton *m_resetBtn;
    QVBoxLayout *m_settingsLayout;
    TranslucentFrame *m_settingsWidget;

    QList<ModuleInterface *> m_moduleInterfaces;
    QMap<ModuleInterface *, bool> m_moduleActivable;
    QMap<ModuleInterface *, QList<ContentWidget *>> m_moduleWidgets;

    QTimer *m_refreshModuleActivableTimer;

    QString m_ensureVisibleModule;
    QString m_ensureVisiblePage;
};

#endif // SETTINGSWIDGET_H
