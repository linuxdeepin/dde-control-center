#ifndef SINGLEMONITORSETTINGS_H
#define SINGLEMONITORSETTINGS_H

#include <QFrame>

class SingleMonitorSettings : public QFrame
{
    Q_OBJECT
public:
    explicit SingleMonitorSettings(QString monitorDBusPath, QWidget *parent = 0);

    void initUI();

private:
    QString m_dbusPath;
};

#endif // SINGLEMONITORSETTINGS_H
