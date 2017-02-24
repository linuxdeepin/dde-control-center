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

    bool exist() const;
    void setExist(bool exist);

signals:
    void existChanged(bool exist);

private:
    WacomModelBase *m_wacomModelBase;
    bool m_exist;
};
}
}

#endif // WACOMMODEL_H
