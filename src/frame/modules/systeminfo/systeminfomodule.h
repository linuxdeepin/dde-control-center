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

#ifndef SYSTEMINFOMODULE_H
#define SYSTEMINFOMODULE_H

#include "interfaces/moduleinterface.h"
#include "systeminfowork.h"
#include "systeminfomodel.h"
#include "systeminfowidget.h"

#include <QObject>

namespace dcc{
namespace systeminfo{

class CopyrightWidget;
class BootWidget;

class SystemInfoModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit SystemInfoModule(FrameProxyInterface* frame, QObject* parent = 0);
    ~SystemInfoModule();

    void initialize();
    void moduleActive();
    void moduleDeactive();
    void reset();
    ModuleWidget *moduleWidget();

    const QString name() const;

    void contentPopped(ContentWidget * const w);

public Q_SLOTS:
    void onPushCopyright();
#ifndef DCC_DISABLE_GRUB
    void onPushBoot();
#endif

    void onSetAutoHide(const bool visible);

private:
    SystemInfoWork* m_work;
    SystemInfoModel* m_model;
#ifndef DCC_DISABLE_GRUB
    BootWidget* m_bootWidget;
#endif
    SystemInfoWidget* m_mainWidget;
    CopyrightWidget* m_copyrightWidget;
};

}
}
#endif // SYSTEMINFOMODULE_H
