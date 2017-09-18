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

#ifndef DISPLAYMODULE_H
#define DISPLAYMODULE_H

#include "moduleinterface.h"

#ifndef DCC_DISABLE_MIRACAST
#include "miracastmodel.h"
#endif

namespace dcc {
class ModuleWidget;

namespace display {
class Monitor;
class DisplayWorker;
class DisplayModel;
class DisplayWidget;
#ifndef DCC_DISABLE_MIRACAST
class MiracastWorker;
#endif
class DisplayModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit DisplayModule(FrameProxyInterface *frame, QObject *parent = 0);
    ~DisplayModule();

private slots:
    void showBrightnessPage();
    void showResolutionDetailPage();
    void showCustomSettings(const QString &config);
    void showConfigPage(const QString &config);
    void showRecognize();
#ifndef DCC_DISABLE_ROTATE
    void showRotate(Monitor *mon = nullptr);
#endif

#ifndef DCC_DISABLE_MIRACAST
    void showMiracastPage(const QDBusObjectPath &path);
#endif

private:
    void initialize();
    void reset();
    void moduleActive();
    void moduleDeactive();
    void contentPopped(ContentWidget * const w);
    const QString name() const;
    ModuleWidget *moduleWidget();

private:
    DisplayModel *m_displayModel;
    DisplayWorker *m_displayWorker;
    DisplayWidget *m_displayWidget;
#ifndef DCC_DISABLE_MIRACAST
    MiracastModel *m_miracastModel;
    MiracastWorker *m_miracastWorker;
#endif
};

} // namespace display

} // namespace dcc

#endif // DISPLAYMODULE_H
