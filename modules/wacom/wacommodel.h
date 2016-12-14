#ifndef WACOMMODEL_H
#define WACOMMODEL_H

#include <QObject>

namespace  dcc
{
namespace wacom
{
class WacomModelBase;
class WacomModel : public QObject
{
    Q_OBJECT
    friend class MouseWorker;
public:
    explicit WacomModel(QObject *parent = 0);
    ~WacomModel();
    WacomModelBase *getWacomModelBase();
    void setIsExist(bool state);
    inline bool isExist(){return m_exist;}

signals:
    void existChanged(bool state);

private:
    WacomModelBase *m_wacomModelBase;
    bool m_exist;
};
}
}

#endif // WACOMMODEL_H
