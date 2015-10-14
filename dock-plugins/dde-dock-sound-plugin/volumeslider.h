#ifndef VOLUMESLIDER_H
#define VOLUMESLIDER_H

#include <QSlider>

class VolumeSlider : public QSlider
{
    Q_OBJECT
    Q_PROPERTY(bool isMute READ isMute WRITE setIsMute)
public:
    explicit VolumeSlider(Qt::Orientation orientation, QWidget * parent = 0);
    ~VolumeSlider();

    bool isMute() const;
    void setIsMute(bool value);

private:
    bool m_isMute = false;
};

#endif // VOLUMESLIDER_H
