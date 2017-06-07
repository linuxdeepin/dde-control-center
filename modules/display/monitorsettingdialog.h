#ifndef MONITORSETTINGDIALOG_H
#define MONITORSETTINGDIALOG_H

#include "monitor.h"
#include "dccslider.h"

#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <ddialog.h>
#include <dimagebutton.h>

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

signals:
    void requestMerge() const;
    void requestSplit() const;
    void requestRecognize() const;
    void requestSetPrimary(const int index) const;
#ifndef DCC_DISABLE_ROTATE
    void requestMonitorRotate(Monitor *mon) const;
#endif
    void requestSetMonitorMode(Monitor *mon, const int mode) const;
//    void requestSetMonitorBrightness(Monitor *mon, const double brightness) const;
    void requestSetMonitorPosition(Monitor *mon, const int x, const int y) const;

private:
    void resizeEvent(QResizeEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

    void init();
    void initPrimary();

    void reloadMonitorObject(Monitor *monitor);
    void reloadOtherScreensDialog();

    void updateModeList(const QList<Resolution> &modeList);

private slots:
    void updateScreensRelation();
    void onPrimaryChanged();
    void onMonitorRectChanged();
    void onMonitorModeChanged();
//    void onMonitorBrightnessChanegd(const double brightness);
    void onMonitorModeSelected(const int index);
#ifndef DCC_DISABLE_ROTATE
    void onRotateBtnClicked();
#endif
//    void onBrightnessSliderChanged(const int value);

private:
    bool m_primary;

    DisplayModel *m_model;
    Monitor *m_monitor;

    widgets::BasicListModel *m_resolutionsModel;

    QLabel *m_monitorName;
#ifndef DCC_DISABLE_ROTATE
    Dtk::Widget::DImageButton *m_rotateBtn;
#endif
//    dcc::widgets::DCCSlider *m_lightSlider;
//    SettingsListWidget *m_resolutionsWidget;
    SettingsListWidget *m_primarySettingsWidget;
    MonitorControlWidget *m_ctrlWidget;
    QHBoxLayout *m_btnsLayout;
    QVBoxLayout *m_mainLayout;

    QTimer *m_smallDelayTimer;

    QList<MonitorSettingDialog *> m_otherDialogs;
};

} // namespace display

} // namespace dcc

#endif // MONITORSETTINGDIALOG_H
