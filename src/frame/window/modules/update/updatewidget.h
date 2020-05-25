/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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
#pragma once

#include "interface/namespace.h"
#include "updatectrlwidget.h"

#include <QObject>
#include <QWidget>
#include <QListView>
#include <QList>

#include <DButtonBox>

class AppUpdateInfo;

QT_BEGIN_NAMESPACE
class QStackedLayout;
class QLayoutItem;
class QVBoxLayout;
QT_END_NAMESPACE

namespace dcc {
namespace update {
class UpdateModel;
class UpdateWorker;
}

namespace widgets {
class SettingsGroup;
}
}

namespace DCC_NAMESPACE {
namespace update {

static const QString OfflineUpgraderService = "com.deepin.dde.OfflineUpgrader";

class UpdateCtrlWidget;
class UpdateHistoryButton;
class RecentHistoryApplist;
class UpdateSettings;

class UpdateWidget : public QWidget
{
    Q_OBJECT
public:
    enum UpdateType {
        Default = -1,
        UpdateCheck,
        UpdateSetting,
        UpdateSettingMir,
        Count
    };
public:
    explicit UpdateWidget(QWidget *parent = nullptr);
    ~UpdateWidget();

    void initialize();
    void setModel(const dcc::update::UpdateModel *model, const dcc::update::UpdateWorker *work);
    void setSystemVersion(QString version);
    void resetUpdateCheckState(bool state = true);
    void mouseDoubleClickEvent(QMouseEvent *event);

    void refreshWidget(UpdateType type = UpdateCheck);

private:
    void displayUpdateContent(UpdateType index);
    void showCheckUpdate();
    void showUpdateSetting();

Q_SIGNALS:
    void topListviewChanged(const QModelIndex &index);
    void pushMirrorsView();

public Q_SLOTS:
    void onNotifyUpdateState(int state);
    void onAppendApplist(const QList<AppUpdateInfo> &infos);

private:
    QVBoxLayout *m_layout;
    dcc::update::UpdateModel *m_model;
    dcc::update::UpdateWorker *m_work;
    QVBoxLayout *m_centerLayout;
    QLabel *m_label;//System Version display
    QString m_systemVersion;
    UpdateHistoryButton *m_historyBtn;//update history button
    dcc::update::UpdatesStatus m_updateState;
    QLabel *m_updateHistoryText;
    dcc::widgets::SettingsGroup *m_applistGroup;
    RecentHistoryApplist *m_recentHistoryApplist;
    DButtonBox *m_topSwitchWidgetBtn;
    QList<DButtonBoxButton *> m_btnlist;
    QStackedLayout *m_mainLayout;
};

}// namespace datetime
}// namespace DCC_NAMESPACE
