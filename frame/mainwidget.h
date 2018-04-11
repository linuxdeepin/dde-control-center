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
#include "indicatorwidget.h"
#include "quick_control/quickcontrolpanel.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

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

signals:
    void showAllSettings() const;
    void showSettingPage(const QString &module, const QString &page, bool animation);

protected:
    void resizeEvent(QResizeEvent *e);

private:
    int getPluginsHeight();
    void updatePluginsHeight();

private slots:
    void pluginAdded(QWidget * const w);
    void showNextPlugin();
    void showPrevPlugin();
    void refershTimedate();
    void updateMPRISEnable();

private:
    PluginsController *m_pluginsController;

//    QWidget *m_lastPluginWidget;
    QTimer *m_timeRefersh;

    // ui widgets
    dcc::accounts::AvatarWidget *m_userAvatarBtn;
    QLabel *m_currentTimeLbl;
    QLabel *m_currentDateLbl;
//    QHBoxLayout *m_pluginsLayout;
    QStackedLayout *m_pluginsLayout;
    QWidget *m_pluginWrapper;
    QWidget *m_pluginWidget;
    IndicatorWidget *m_indicatorWidget;
#ifndef DISABLE_SYS_UPDATE
    UpdateNotifier *m_updateNotifier;
#endif
    QuickControlPanel *m_quickSettingsPanel;
};

#endif // MAINWIDGET_H
