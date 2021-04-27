#include "customping.h"
#include <QDebug>

CustomPing::CustomPing()
{
    m_process = new QProcess();
    m_process->start(m_cmdstr);
}

QProcess *CustomPing::getProcess()
{
    return m_process;
}

void CustomPing::run()
{
    exec();
}

void CustomPing::slot_resetProcess()
{
    msleep(50);
    m_process->start();
}
