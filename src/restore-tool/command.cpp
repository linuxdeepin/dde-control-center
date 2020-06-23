#include "command.h"

#include <QProcess>
#include <QDebug>
#include <QThread>

bool SpawnCmd(const QString& cmd, const QStringList& args) {
  QProcess process;
  process.setProgram(cmd);
  process.setArguments(args);
  // Merge stdout and stderr of subprocess with main process.
  process.setProcessChannelMode(QProcess::MergedChannels);
  process.start();
  // Wait for process to finish without timeout.
  process.waitForFinished(-1);
  return (process.exitStatus() == QProcess::NormalExit &&
          process.exitCode() == 0);
}

bool SpawnCmd(const QString& cmd, const QStringList& args, QString& output) {
  QString err;
  return SpawnCmd(cmd, args, output, err);
}

bool SpawnCmd(const QString& cmd, const QStringList& args,
              QString& output, QString& err) {
    QProcess process;
    process.setProgram(cmd);
    process.setArguments(args);

    process.start();
    // Wait for process to finish without timeout.
    process.waitForFinished(-1);
    output += process.readAllStandardOutput();
    err += process.readAllStandardError();

    if (process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0) {
        return true;
    }

    return false;
}

bool SpawnCmd(const QString& cmd, const QStringList& args,
              QString& output, QString& err, int tryCount) {
    int loop_num = 0;
    while (loop_num++ < tryCount) {
        bool result = SpawnCmd(cmd, args, output, err);
        if (result) {
            return true;
        }

        qDebug() << err;
        qDebug() << "try again!!!!!";
        QThread::sleep(1);
    }

    return false;
}
