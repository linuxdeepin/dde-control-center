#ifndef MONITOR_H
#define MONITOR_H

#include <QLabel>

class Monitor : public QLabel
{
    Q_OBJECT
public:
    explicit Monitor(QWidget *parent = 0);

    void setName(QString name);

    QSize resolution();
    void setResolution(int width, int height);

private:
    QSize m_resolution;
};

#endif // MONITOR_H
