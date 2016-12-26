#ifndef MOUSEMODELBASE_H
#define MOUSEMODELBASE_H

#include <QObject>
#include <QDebug>

namespace dcc
{
namespace mouse
{
class MouseModelBase : public QObject
{
    Q_OBJECT
public:
    explicit MouseModelBase(QObject *parent = 0);
    void setSliderValue(const int &value);
    inline bool getSliderValue() {return m_sliderValue;}
    void setExist(bool state);
    inline bool getExist(){return m_isExist;}

signals:
    void sliderValueChanged(const int &value);
    void existChanged(bool state);

private:
    int m_sliderValue;
    bool m_isExist;
};
}
}

#endif // MOUSEMODELBASE_H
