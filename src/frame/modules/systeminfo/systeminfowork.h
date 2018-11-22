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

#ifndef SYSTEMINFOWORK_H
#define SYSTEMINFOWORK_H

#include <QObject>
#include <com_deepin_daemon_systeminfo.h>
#include <com_deepin_daemon_grub2.h>
#include <com_deepin_daemon_grub2_theme.h>

using SystemInfoInter=com::deepin::daemon::SystemInfo;

#ifndef DCC_DISABLE_GRUB
using GrubDbus=com::deepin::daemon::Grub2;
using GrubThemeDbus = com::deepin::daemon::grub2::Theme;
#endif

namespace dcc{
namespace systeminfo{

class SystemInfoModel;

class SystemInfoWork : public QObject
{
    Q_OBJECT

public:
    explicit SystemInfoWork(SystemInfoModel* model, QObject* parent = 0);

    void activate();
    void deactivate();

#ifndef DCC_DISABLE_GRUB
    void loadGrubSettings();

Q_SIGNALS:
    void requestSetAutoHideDCC(const bool visible) const;

public Q_SLOTS:
    void setBootDelay(bool value);
    void setEnableTheme(bool value);
    void setDefaultEntry(const QString& entry);
    void grubServerFinished();
    void onBackgroundChanged();
    void setBackground(const QString &path);

private:
    void getEntryTitles();
    void getBackgroundFinished(QDBusPendingCallWatcher *w);

#endif

private:
    SystemInfoModel* m_model;
    SystemInfoInter* m_systemInfoInter;
#ifndef DCC_DISABLE_GRUB
    GrubDbus* m_dbusGrub;
    GrubThemeDbus *m_dbusGrubTheme;
#endif
};

}
}
#endif // SYSTEMINFOWORK_H
