#ifndef DCCSLIDER_H
#define DCCSLIDER_H

#include <QSlider>

namespace dcc {

class DCCSlider : public QSlider
{
    Q_OBJECT
public:
    explicit DCCSlider(QWidget *parent = 0);

protected:
    void wheelEvent(QWheelEvent *e);
};

}

#endif // DCCSLIDER_H
