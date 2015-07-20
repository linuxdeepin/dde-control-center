#include "diskicon.h"

DiskIcon::DiskIcon(QWidget *parent) : QLabel(parent)
{
    setFixedSize(48,48);
}

void DiskIcon::setIcon(const QString &icon)
{
    QString tmpUrl = getIconUrl(icon);
    if (tmpUrl != "")
    {
        QPixmap iconMap(tmpUrl);
        setPixmap(iconMap.scaled(size()));
    }
    else
        setPixmap(QIcon::fromTheme(icon).pixmap(size()));
}

void DiskIcon::setMountPoint(const QString &path)
{
    m_mountPoint = path;
}

void DiskIcon::mousePressEvent(QMouseEvent *)
{
    QProcess * tmpProcess = new QProcess();
    connect(tmpProcess, SIGNAL(finished(int)), tmpProcess, SLOT(deleteLater()));
    tmpProcess->start("xdg-open " + m_mountPoint);
}

QString DiskIcon::getIconUrl(const QString &icon)
{
    QString tmpUrl = QString("://Resource/images/%1.png").arg(icon);

    if (QFile::exists(tmpUrl))
        return tmpUrl;
    else
        return "";
}
