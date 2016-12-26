#ifndef DCCSLIDER_H
#define DCCSLIDER_H

#include <QSlider>

namespace dcc {
namespace widgets {

class DCCSlider : public QSlider
{
    Q_OBJECT
public:
    enum SliderType {
        Normal,
        Vernier,
        Progress
    };

    explicit DCCSlider(SliderType type = Normal, QWidget *parent = 0);

    void setType(SliderType type);

protected:
    void wheelEvent(QWheelEvent *e);
};

}
}

#endif // DCCSLIDER_H
