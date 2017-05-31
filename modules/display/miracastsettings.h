#ifndef MIRACASTSETTINGS_H
#define MIRACASTSETTINGS_H

#include "contentwidget.h"
#include "switchwidget.h"
#include "settingsgroup.h"
#include "miracastnodevicepage.h"
#include "miracastmodel.h"
#include "QVBoxLayout"

using namespace dcc::widgets;

namespace dcc {

namespace display {

class MiracastPage : public ContentWidget
{
    Q_OBJECT
public:
    explicit MiracastPage(const QString &title, QWidget *parent = 0);

signals:
    void requestSwitchDevice(const bool state);

private:
    QVBoxLayout *m_mainLayout;
    MiracastNoDevicePage *m_nodevice;
    SettingsGroup *m_deviceGrp;
};

}
}

#endif // MIRACASTSETTINGS_H
