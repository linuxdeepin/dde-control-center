#ifndef RESOURCE_H
#define RESOURCE_H

#include <QObject>

class IconResource : public QObject
{
    Q_OBJECT
public:
    IconResource(QObject *parent = 0): QObject(parent)
    {

    }

    Q_INVOKABLE QString getIconUrl(QString path)
    {
        return "file:///usr/share/dde-control-center/dock-applets/icons/" + path;
    }
};

#endif // RESOURCE_H
