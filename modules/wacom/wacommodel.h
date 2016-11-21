#ifndef WACOMMODEL_H
#define WACOMMODEL_H

#include "model/wacommodelbase.h"

#include <QObject>
#include <QDebug>


class WacomModel : public QObject
{
    Q_OBJECT
    friend class MouseWorker;
public:
    explicit WacomModel(QObject *parent = 0);
    ~WacomModel();
    WacomModelBase *getWacomModelBase();

private:
    WacomModelBase *m_wacomModelBase;

};

#endif // WACOMMODEL_H
