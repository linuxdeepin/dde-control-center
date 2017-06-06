#ifndef MIRACASTNODEVICEWIDGET_H
#define MIRACASTNODEVICEWIDGET_H

#include "translucentframe.h"
#include "loadingindicator.h"
#include "../widgets/labels/normallabel.h"
#include <dimagebutton.h>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;

namespace dcc {
namespace display {

class MiracastNoDeviceWidget : public TranslucentFrame
{
    Q_OBJECT
public:
    explicit MiracastNoDeviceWidget(QWidget *parent = 0);

    enum DeviceListState{
        NoDevice,
        Refreshed
    };

signals:
    void requestRefreshed() const;

public slots:
    void setState(DeviceListState state);

protected:
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

private:
    void refresh();
    void nodevice();

private:
    LoadingIndicator *m_loading;
    NormalLabel *m_refreshTip;
    NormalLabel *m_nodeviceTip;
    DImageButton *m_refreshBtn;
};

}
}

#endif // MIRACASTNODEVICEWIDGET_H
