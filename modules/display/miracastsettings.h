#ifndef MIRACASTSETTINGS_H
#define MIRACASTSETTINGS_H

#include "contentwidget.h"
#include "switchwidget.h"
#include "settingsgroup.h"
#include "miracastnodevicepage.h"
#include "miracastdevicemodel.h"
#include "miracastitem.h"
#include "QVBoxLayout"

using namespace dcc::widgets;

namespace dcc {

namespace display {

class MiracastPage : public ContentWidget
{
    Q_OBJECT
public:
    explicit MiracastPage(const QString &title, QWidget *parent = 0);
    void setModel(MiracastDeviceModel *model);

signals:
    void requestSwitchDevice(const bool state);

private slots:
    void onItemAdded(MiracastItem *item);
    void onItemRemoved(MiracastItem *item);

private:
    QVBoxLayout *m_mainLayout;
    MiracastNoDevicePage *m_nodevice;
    SettingsGroup *m_deviceGrp;
    MiracastDeviceModel *m_model;
};

}
}

#endif // MIRACASTSETTINGS_H
