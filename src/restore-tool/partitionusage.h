#ifndef PARTITIONUSAGE_H
#define PARTITIONUSAGE_H
#include "filesystem.h"

#include <QString>

// Read partition usage at |partition_path| with type |fs_type|
// Returns false if failed, both |freespace| and |total| are set to -1.
bool ReadUsage(const QString& partition_path,
               FsType fs_type,
               qint64& freespace,
               qint64& total);

#endif // PARTITIONUSAGE_H
