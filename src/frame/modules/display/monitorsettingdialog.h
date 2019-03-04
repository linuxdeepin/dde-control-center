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

#ifndef MONITORSETTINGDIALOG_H
#define MONITORSETTINGDIALOG_H

#include "monitor.h"
#include "widgets/dccslider.h"
#include "monitorindicator.h"

#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <ddialog.h>
#include <dimagebutton.h>
#include <memory>

DWIDGET_USE_NAMESPACE

namespace dcc {

namespace widgets {

class BasicListView;
class BasicListModel;

}

namespace display {

class DisplayModel;
class SettingsListWidget;
class MonitorControlWidget;
class MonitorSettingDialog : public DAbstractDialog
{
    Q_OBJECT

public:
    explicit MonitorSettingDialog(DisplayModel *model, QWidget *parent = nullptr);
    explicit MonitorSettingDialog(Monitor *monitor, QWidget *parent = nullptr);
    ~MonitorSettingDialog();

Q_SIGNALS:
    void requestMerge() const;
    void requestSplit() const;
    void requestRecognize() const;
    void requestSetPrimary(const int index) const;
#ifndef DCC_DISABLE_ROTATE
    void requestMonitorRotate(Monitor *mon) const;
#endif
    void requestSetMonitorResolution(Monitor *mon, const int mode) const;
    void requestSetMonitorPosition(Monitor *mon, const int x, const int y) const;
    void requestApplySave() const;

private:
    void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

    void init();
    void initPrimary();

    void reloadMonitorObject(Monitor *monitor);
    void reloadOtherScreensDialog();

    void updateModeList(const QList<Resolution> &modeList);

private Q_SLOTS:
    void updateScreensRelation();
    void onPrimaryChanged();
    void onMonitorRectChanged();
    void onMonitorModeChanged();
    void onMonitorResolutionSelected(const int index);
#ifndef DCC_DISABLE_ROTATE
    void onRotateBtnClicked();
#endif
    void onMonitorPress(Monitor *mon);
    void onMonitorRelease(Monitor *mon);

private:
    bool m_primary;

    DisplayModel *m_model;
    Monitor *m_monitor;

    widgets::BasicListModel *m_resolutionsModel;

#ifndef DCC_DISABLE_ROTATE
    Dtk::Widget::DImageButton *m_rotateBtn;
#endif
    SettingsListWidget *m_primarySettingsWidget;
    MonitorControlWidget *m_ctrlWidget;
    std::unique_ptr<MonitorIndicator> m_fullIndication;
    QHBoxLayout *m_btnsLayout;
    QVBoxLayout *m_mainLayout;

    QTimer *m_positionWatcher;

    QList<MonitorSettingDialog *> m_otherDialogs;
};

} // namespace display

} // namespace dcc

#endif // MONITORSETTINGDIALOG_H
