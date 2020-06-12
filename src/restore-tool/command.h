#ifndef COMMAND_H
#define COMMAND_H

#include <QStringList>

// Run |cmd| with |args| in background and returns its result.
bool SpawnCmd(const QString& cmd, const QStringList& args);
bool SpawnCmd(const QString& cmd, const QStringList& args, QString& output);
bool SpawnCmd(const QString& cmd, const QStringList& args, QString& output,
              QString& err);
bool SpawnCmd(const QString& cmd, const QStringList& args, QString& output,
              QString& err, int tryCount);

#endif // COMMAND_H
