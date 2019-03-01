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

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "pluginscontroller.h"
#include "framewidget.h"
#include "navigation/navwidget.h"
#include "notify/notifywidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QStackedLayout>

#include <dimagebutton.h>
#include <dpageindicator.h>

namespace dcc {
namespace accounts {
    class AvatarWidget;
}
}

class UpdateNotifier;
class MainWidget : public FrameWidget
{
    Q_OBJECT

public:
    explicit MainWidget(FrameContentWrapper *parent = 0);

Q_SIGNALS:
    void showAllSettings() const;
    void showSettingPage(const QString &module, const QString &page, bool animation);
    void appear() const;
    void disappear() const;
    void requestModuleVisible(const QString &module, bool visible);

private:
    int getPluginsHeight();

private Q_SLOTS:
    void pluginAdded(QWidget * const w);
    void showNextPlugin();
    void showPrevPlugin();
    void refershTimedate();
    void toggleNotify();

private:
    PluginsController *m_pluginsController;

    QTimer *m_timeRefersh;

    // ui widgets
    dcc::accounts::AvatarWidget *m_userAvatarBtn;
    QLabel *m_currentTimeLbl;
    QLabel *m_currentDateLbl;
    DImageButton *m_notifyToggleBtn;
    QStackedLayout *m_pluginsLayout;
    NavWidget *m_navWidget;
    NotifyWidget *m_notifyWidget;
#ifndef DISABLE_SYS_UPDATE
    UpdateNotifier *m_updateNotifier;
#endif
};

#endif // MAINWIDGET_H
