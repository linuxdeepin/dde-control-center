/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
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

#ifndef CUSTOMSETTINGDIALOG_H
#define CUSTOMSETTINGDIALOG_H

#include "window/namespace.h"

#include <DListView>
#include <DSegmentedControl>

#include <QDialog>

#include <memory>

QT_BEGIN_NAMESPACE
class QListView;
class QVBoxLayout;
QT_END_NAMESPACE

namespace dcc {

namespace display {
class DisplayModel;
class Monitor;
class MonitorControlWidget;
class MonitorIndicator;
}

namespace widgets {
class BasicListView;
}
}

namespace DCC_NAMESPACE {

namespace display {

class CustomSettingDialog : public QDialog
{
    Q_OBJECT
public:
    CustomSettingDialog(QWidget *parent = nullptr);
    CustomSettingDialog(dcc::display::Monitor *mon, dcc::display::DisplayModel *model, QWidget *parent = nullptr);
    ~CustomSettingDialog();

public:
    void setModel(dcc::display::DisplayModel *model);

Q_SIGNALS:
    void requestShowRotateDialog(dcc::display::Monitor *mon);
    void requestRecognize();
    void requestMerge();
    void requestSplit();
    void requestSetMonitorPosition(dcc::display::Monitor *mon, const int x, const int y);
    void requestSetResolution(dcc::display::Monitor *mon, int resolution);
    void requestSetPrimaryMonitor(int idx);
    void requestApplySave();


private Q_SLOTS:
    void onMonitorPress(dcc::display::Monitor *mon);
    void onMonitorRelease(dcc::display::Monitor *mon);
    void resetDialog();
    void onPrimaryMonitorChanged();
    void onChangList();

private:
    void initUI();
    void initWithModel();
    void initOtherDialog();
    void initRefreshrateList();
    void initResolutionList();
    void initMoniList();
    void initMoniControlWidget();
    //初始化主屏窗口中的　屏幕代理　及　屏幕列表　部分．
    void initPrimaryDialog();

private:
    bool m_isPrimary{false};
    dcc::display::Monitor *m_monitor{nullptr};
    dcc::display::DisplayModel *m_model{nullptr};
    QVBoxLayout *m_layout{nullptr};
    std::unique_ptr<dcc::display::MonitorIndicator> m_fullIndication;
    dcc::display::MonitorControlWidget *m_monitroControlWidget{nullptr};

    QVBoxLayout *m_listLayout{nullptr};
    DTK_WIDGET_NAMESPACE::DSegmentedControl *m_segmentBtn{nullptr};
    DTK_WIDGET_NAMESPACE::DListView *m_moniList{nullptr};
    DTK_WIDGET_NAMESPACE::DListView *m_resolutionList{nullptr};
    DTK_WIDGET_NAMESPACE::DListView *m_rateList{nullptr};
    QStandardItemModel *m_resolutionListModel{nullptr};
    QList<CustomSettingDialog *> m_otherDialog;
};
}

}

#endif // CUSTOMSETTINGDIALOG_H
