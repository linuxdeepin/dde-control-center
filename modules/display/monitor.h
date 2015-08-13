#ifndef MONITOR_H
#define MONITOR_H

#include <QLabel>

class Monitor : public QLabel
{
    Q_OBJECT
public:
    explicit Monitor(QWidget *parent = 0);
};

#endif // MONITOR_H
