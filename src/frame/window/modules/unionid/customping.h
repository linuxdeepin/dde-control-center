#ifndef MYPING_H
#define MYPING_H

#include <QProcess>
#include <QThread>

class CustomPing : public QThread
{
public:
    CustomPing();

    QProcess *getProcess();

    void run();

public Q_SLOTS:
    void slot_resetProcess();

private:
    QString m_cmdstr = "ping www.baidu.com";
    QProcess *m_process;
};

#endif // MYPING_H
