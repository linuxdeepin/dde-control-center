#ifndef WIREDPAGE_H
#define WIREDPAGE_H

#include "contentwidget.h"

namespace dcc {

namespace widgets {
class SettingsGroup;
}

namespace network {

class WiredDevice;
class NetworkModel;
class WiredPage : public ContentWidget
{
    Q_OBJECT
public:
    explicit WiredPage(WiredDevice *dev, QWidget *parent = 0);

    void setModel(NetworkModel *model);

private slots:
    void initUI();

private:
    WiredDevice *m_device;
    NetworkModel *m_model;

    widgets::SettingsGroup *m_settingsGrp;
};

}

}

#endif // WIREDPAGE_H
