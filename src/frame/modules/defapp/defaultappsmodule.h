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

#ifndef DEFAULTAPPSMODULE_H
#define DEFAULTAPPSMODULE_H

#include "interfaces/moduleinterface.h"

namespace dcc {
namespace defapp {
class DefAppModel;
class DefAppWorker;
class DefaultAppsWidget;
class DefAppViewer;
class DefaultAppsModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit DefaultAppsModule(FrameProxyInterface *frame, QObject *parent = 0);
    ~DefaultAppsModule();

    void initialize();
    void moduleActive();
    void moduleDeactive();
    void reset();

    ModuleWidget *moduleWidget();

    const QString name() const;

Q_SIGNALS:
    void requestReset();

private:
    void showDefaultAppsDetail();
    void contentPopped(ContentWidget * const w);
    void setFrameAutoHide(const bool autoHide);

private:
    DefAppModel       *m_defAppModel;
    DefAppWorker      *m_defAppWorker;
    DefaultAppsWidget *m_defaultappsWidget;
    DefAppViewer      *m_defaultAppsDetail;
};
}
}
#endif // DEFAULTAPPSMODULE_H
