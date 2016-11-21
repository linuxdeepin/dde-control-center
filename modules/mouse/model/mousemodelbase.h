#ifndef MOUSEMODELBASE_H
#define MOUSEMODELBASE_H

#include <QObject>
#include <QDebug>

class MouseModelBase : public QObject
{
    Q_OBJECT
public:
    explicit MouseModelBase(QObject *parent = 0);
    void setSliderValue(const int &value);
    inline bool getSliderValue() {return m_sliderValue;}

signals:
    void sliderValueChanged(const int &value);

private:
    int m_sliderValue;
};
#endif // MOUSEMODELBASE_H
