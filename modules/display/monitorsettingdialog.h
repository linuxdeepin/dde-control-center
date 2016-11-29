#ifndef MONITORSETTINGDIALOG_H
#define MONITORSETTINGDIALOG_H

#include "monitor.h"
#include "displaymodel.h"
#include "dccslider.h"

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
    explicit MonitorSettingDialog(DisplayModel *model, Monitor *monitor, QWidget *parent = 0);

    void setPrimary(const bool primary = true);

private slots:
    void onMonitorRectChanged();

private:
    bool m_primary;

    DisplayModel *m_model;
    Monitor *m_monitor;

    Dtk::Widget::DImageButton *m_rotateBtn;
    dcc::DCCSlider *m_lightSlider;
    QHBoxLayout *m_btnsLayout;
};

#endif // MONITORSETTINGDIALOG_H
