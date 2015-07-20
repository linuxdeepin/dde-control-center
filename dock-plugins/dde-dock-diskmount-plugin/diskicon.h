#ifndef DISKICON_H
#define DISKICON_H

#include <QWidget>
#include <QLabel>
#include <QFile>
#include <QPixmap>
#include <QIcon>
#include <QProcess>
#include <QDebug>

class DiskIcon : public QLabel
{
    Q_OBJECT
public:
    explicit DiskIcon(QWidget *parent = 0);

    void setIcon(const QString &icon);
    void setMountPoint(const QString &path);

protected:
    void mousePressEvent(QMouseEvent * event);

private:
    QString getIconUrl(const QString &icon);

private:
    QString m_mountPoint = "";
};

#endif // DISKICON_H
