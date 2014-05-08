#ifndef RESOURCE_H
#define RESOURCE_H

#include <QObject>
#include <QDebug>

class IconResource : public QObject
{
    Q_OBJECT
public:
    IconResource(QObject *parent = 0): QObject(parent)
    {

    }

    Q_INVOKABLE QString getIconUrl(QString path)
    {
        QString iconPath = "file://"DOCKAPPLETSDIR"/icons/" + path;
        return iconPath;
    }
};

#endif // RESOURCE_H
