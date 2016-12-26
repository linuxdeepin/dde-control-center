#ifndef WACOMMODELBASE_H
#define WACOMMODELBASE_H

#include <QObject>
#include <QDebug>
namespace dcc
{
namespace wacom
{
class WacomModelBase : public QObject
{
    Q_OBJECT
public:
    explicit WacomModelBase(QObject *parent = 0);
    void setPressureValue(const int &value);
    inline bool getPressureValue() {return m_pressureValue;}

signals:
    void pressureValueChanged(const int &value);

private:
    int m_pressureValue;
};
}
}

#endif // WACOMMODELBASE_H
