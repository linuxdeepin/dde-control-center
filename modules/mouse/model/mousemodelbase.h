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

    bool getNaturalScroll() const;
    void setNaturalScroll(bool naturalScroll);

signals:
    void sliderValueChanged(const int &value);
    void existChanged(bool state);
    void naturalScrollChanged(bool state);

private:
    int m_sliderValue;
    bool m_isExist;
    bool m_naturalScroll;
};
}
}

#endif // MOUSEMODELBASE_H
