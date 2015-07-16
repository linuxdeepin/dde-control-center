#ifndef DEVICEFRAME_H
#define DEVICEFRAME_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "dbus/dbusaudio.h"
#include "deviceiconslider.h"

class DeviceFrame : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceFrame(QWidget *parent = 0);

signals:
    void heightChanged(int height);

public slots:

private:
    void initTitle();
    void initDevice();

    QVBoxLayout * m_mainLayout = NULL;
};

#endif // DEVICEFRAME_H
