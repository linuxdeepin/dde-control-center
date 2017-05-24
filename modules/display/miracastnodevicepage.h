#ifndef MIRACASTNODEVICEPAGE_H
#define MIRACASTNODEVICEPAGE_H

#include "translucentframe.h"

using namespace dcc::widgets;

namespace dcc {
namespace display {

class MiracastNoDevicePage : public TranslucentFrame
{
    Q_OBJECT
public:
    explicit MiracastNoDevicePage(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;
};

}
}

#endif // MIRACASTNODEVICEPAGE_H
