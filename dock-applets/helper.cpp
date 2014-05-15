#include "helper.h"

#include <QProcess>

ExternalObject::ExternalObject(QObject *parent)
    :QObject(parent)
{

}

QString ExternalObject::getIconUrl(QString path)
{
    QString iconPath = "file://"DOCKAPPLETSDIR"/icons/" + path;
    return iconPath;
}

void ExternalObject::xdgOpen(QString path)
{
    QProcess::execute("xdg-open " + path);
}
