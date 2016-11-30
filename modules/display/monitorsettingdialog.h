#ifndef MONITORSETTINGDIALOG_H
#define MONITORSETTINGDIALOG_H

#include "monitor.h"
#include "displaymodel.h"
#include "dccslider.h"
#include "settingslistwidget.h"

#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>

#include <dimagebutton.h>

using Dtk::Widget::DImageButton;

using dcc::DCCSlider;

class MonitorSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MonitorSettingDialog(DisplayModel *model, QWidget *parent = nullptr);
    explicit MonitorSettingDialog(Monitor *monitor, QWidget *parent = nullptr);
    ~MonitorSettingDialog();

private:
    void init();
    void initPrimary();

private slots:
    void onMonitorRectChanged();
    void onMonitorModeListChanged(const QList<Resolution> &modeList);

private:
    bool m_primary;

    DisplayModel *m_model;
    Monitor *m_monitor;

    Dtk::Widget::DImageButton *m_rotateBtn;
    dcc::DCCSlider *m_lightSlider;
    SettingsListWidget *m_resolutionsWidget;
    QHBoxLayout *m_btnsLayout;

    QList<MonitorSettingDialog *> m_otherDialogs;
};

#endif // MONITORSETTINGDIALOG_H
