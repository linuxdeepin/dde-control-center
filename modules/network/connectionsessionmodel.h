#ifndef CONNECTIONSESSIONMODEL_H
#define CONNECTIONSESSIONMODEL_H

#include <QObject>

namespace dcc {

namespace network {

class ConnectionSessionModel : public QObject
{
    Q_OBJECT

public:
    explicit ConnectionSessionModel(QObject *parent = 0);
};

}

}

#endif // CONNECTIONSESSIONMODEL_H
