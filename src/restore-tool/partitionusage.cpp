#include "partitionusage.h"
#include "command.h"

#include <QRegularExpression>
#include <QFile>

enum class SwapType {
  Partition,
  File,
};

struct SwapItem {
  QString filename;
  SwapType type;
  qint64 size;
  qint64 used;
  int priority;
};

typedef QList<SwapItem> SwapItemList;

QString ReadFile(const QString& path) {
  QFile file(path);
  if (file.exists()) {
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      qWarning() << "ReadFile() failed to open" << path;
      return "";
    }
    const QString &str = QString::fromUtf8(file.readAll());
    file.close();
    return str;
  } else {
    qWarning() << "ReadFileContent() file not found: " << path;
    return "";
  }
}
// Parse /proc/swaps file.
SwapItemList ParseSwaps() {
  SwapItemList result;
  const QString content(ReadFile("/proc/swaps"));

  for (const QString& line : content.split('\n')) {
    if ((!line.isEmpty()) && (!line.startsWith("Filename"))) {
      const QStringList parts(line.split(QRegExp("\\s+")));
      if (parts.length() == 5) {
        SwapItem item = {
            parts.at(0),
            parts.at(1) == "partition" ? SwapType::Partition : SwapType::File,
            parts.at(2).toLongLong() * 1000,  // from kilobytes
            parts.at(3).toLongLong() * 1000,  // from kilobytes
            parts.at(4).toInt(),
        };
        result.append(item);
      }
    }
  }

  return result;
}

// Size units defined in bytes
// See https://en.wikipedia.org/wiki/Kibibyte
const qint64 kKibiByte = 1024;
const qint64 kMebiByte = kKibiByte * kKibiByte;
const qint64 kGibiByte = kMebiByte * kKibiByte;
const qint64 kTebiByte = kGibiByte * kKibiByte;
const qint64 kPebiByte = kTebiByte * kKibiByte;
const qint64 kExbiByte = kPebiByte * kKibiByte;

QString RegexpLabel(const QString& pattern, const QString& str) {
  QRegularExpression reg(pattern, QRegularExpression::MultilineOption);
  QRegularExpressionMatch match = reg.match(str);
  if (match.hasMatch()) {
    return match.captured(1);
  } else {
    return QString();
  }
}

qint64 ParseBtrfsUnit(const QString& value) {
  const float pref = RegexpLabel("^(\\d+\\.?\\d+)", value).toFloat();
  if (value.contains("KiB")) {
    return static_cast<qint64>(pref * kKibiByte);
  }
  if (value.contains("MiB")) {
    return static_cast<qint64>(pref * kMebiByte);
  }
  if (value.contains("GiB")) {
    return static_cast<qint64>(pref * kGibiByte);
  }
  if (value.contains("TiB")) {
    return static_cast<qint64>(pref * kTebiByte);
  }
  if (value.contains("PiB")) {
    return static_cast<qint64>(pref * kPebiByte);
  }
  return -1;
}

bool ReadBtrfsUsage(const QString& path, qint64& freespace, qint64& total) {
  QString output;
  if (!SpawnCmd("btrfs", {"filesystem", "show", path}, output)) {
    return false;
  }
  QString total_str, used_str;
  for (const QString& line : output.split('\n')) {
    if (line.contains(path)) {
      total_str = RegexpLabel("size\\s*([^\\s]*)\\s", line);
    } else if (line.contains("Total devices")) {
      used_str = RegexpLabel("used\\s*([^\\s]*)", line);
    }
  }

  total = ParseBtrfsUnit(total_str);
  freespace = total - ParseBtrfsUnit(used_str);
  return (total > -1 && freespace > -1);
}


bool ReadExt2Usage(const QString& path, qint64& freespace, qint64& total) {
  QString output;
  if (!SpawnCmd("dumpe2fs", {"-h", path}, output)) {
    return false;
  }

  int block_size = 0;
  qint64 total_blocks = 0;
  qint64 free_blocks = 0;
  for (const QString& line : output.split('\n')) {
    if (line.contains("Block count:")) {
      const QString m = RegexpLabel("Block count:\\s+(\\d+)", line);
      if (!m.isEmpty()) {
        total_blocks = m.toLongLong();
      }
    } else if (line.contains("Free blocks:")) {
      const QString m = RegexpLabel("Free blocks:\\s+(\\d+)", line);
      if (!m.isEmpty()) {
        free_blocks = m.toLongLong();
      }
    } else if (line.contains("Block size:")) {
      const QString m = RegexpLabel("Block size:\\s+(\\d+)", line);
      if (!m.isEmpty()) {
        block_size = m.toInt();
      }
    }
  }

  freespace = block_size * free_blocks;
  total = block_size * total_blocks;
  return true;
}

bool ReadFat16Usage(const QString& path, qint64& freespace, qint64& total) {
  QString output, err;
  SpawnCmd("dosfsck", {"-n", "-v", path}, output, err);
  // NOTE(xushaohua): `dosfsck` returns 1 on success, so we check its error
  // message instead.
  if (!err.isEmpty()) {
    qWarning() << "dosfsck failed:" << err;
    return false;
  }

  int cluster_size = 0;
  qint64 start_byte = 0;
  qint64 total_clusters = 0;
  qint64 used_clusters = 0;

  for (const QString& line : output.split('\n')) {
    if (line.contains("bytes per cluster")) {
      cluster_size = line.trimmed().split(' ').at(0).trimmed().toInt();
    } else if (line.contains("Data area starts at")) {
      start_byte = line.split(' ').at(5).toLongLong();
    } else if (line.contains(path)) {
      const QStringList parts = line.split(' ').at(3).split('/');
      total_clusters = parts.at(1).toLongLong();
      used_clusters = parts.at(0).toLongLong();
    }
  }

  total = total_clusters * cluster_size;
  freespace = total - start_byte - used_clusters * cluster_size;
  return true;
}

bool ReadJfsUsage(const QString& path, qint64& freespace, qint64& total) {
  QString output;
  const QString param(QString("echo dm | jfs_debugfs %1").arg(path));
  if (!SpawnCmd("sh", {"-c", param}, output)) {
    return false;
  }

  int block_size = 0;
  qint64 total_blocks = 0;
  qint64 free_blocks = 0;
  for (const QString& line : output.split('\n')) {
    if (line.startsWith("Aggregate Block Size:")) {
      block_size = line.split(':').at(1).trimmed().toInt();
    } else if (line.contains("dn_mapsize:")) {
      const QString item = RegexpLabel("dn_mapsize:\\s*([^\\s]+)", line);
      total_blocks = item.toLongLong(nullptr, 16);
    } else if (line.contains("dn_nfree:")) {
      const QString item = RegexpLabel("dn_nfree:\\s*([^\\s]+)", line);
      free_blocks = item.toLongLong(nullptr, 16);
    }
  }

  if (free_blocks > 0 && total_blocks > 0 && block_size > 0) {
    freespace = free_blocks * block_size;
    total = total_blocks * block_size;
    return true;
  } else {
    return false;
  }
}

bool ReadLinuxSwapUsage(const QString& path, qint64& freespace, qint64& total) {
  const SwapItemList swap_items = ParseSwaps();
  // If this swap partition is used, read from /proc/swaps.
  for (const SwapItem& item : swap_items) {
    if (item.filename == path) {
      total = item.size;
      freespace = item.size - item.used;
      return true;
    }
  }

  // If it is not used, it is totally free.
  freespace = 0;
  total = 0;
  return true;
}

bool ReadNilfs2Usage(const QString& path, qint64& freespace, qint64& total) {
  QString output;
  if (!SpawnCmd("nilfs-tune", {"-l", path}, output)) {
    return false;
  }

  int block_size = 0;
  qint64 free_blocks = 0;
  for (const QString& line : output.split('\n')) {
    if (line.startsWith("Block size:")) {
      block_size = line.split(':').last().trimmed().toInt();
    } else if (line.startsWith("Device size:")) {
      total = line.split(':').last().trimmed().toLongLong();
    } else if (line.startsWith("Free blocks count:")) {
      free_blocks = line.split(':').last().trimmed().toLongLong();
    }
  }

  if (free_blocks > 0 && block_size > 0) {
    freespace = free_blocks * block_size;
    return true;
  } else {
    return false;
  }
}

bool ReadNTFSUsage(const QString& path, qint64& freespace, qint64& total) {
  QString output;
  if (!SpawnCmd("ntfsinfo", {"-mf", path}, output)) {
    return false;
  }
  int cluster_size = 0;
  qint64 free_clusters = 0;
  qint64 total_clusters = 0;
  for (const QString& line : output.split('\n')) {
    if (line.contains("Cluster Size:")) {
      const QString m = RegexpLabel("Cluster Size:\\s+(\\d+)", line);
      if (!m.isEmpty()) {
        cluster_size = m.toInt();
      }
    } else if (line.contains("Volume Size in Clusters:")) {
      const QString m = RegexpLabel("Volume Size in Clusters:\\s+(\\d+)",
                                          line);
      if (!m.isEmpty()) {
        total_clusters = m.toLongLong();
      }
    } else if (line.contains("Free Clusters:")) {
      const QString m = RegexpLabel("Free Clusters:\\s+(\\d+)", line);
      if (!m.isEmpty()) {
        free_clusters = m.toLongLong();
      }
    }
  }

  if (free_clusters > 0 && total_clusters > 0 && cluster_size > 0) {
    freespace = cluster_size * free_clusters;
    total = cluster_size * total_clusters;
    return true;
  } else {
    return false;
  }
}

bool ReadReiser4Usage(const QString& path, qint64& freespace, qint64& total) {
  QString output;
  if (!SpawnCmd("debugfs.reiser4", {"--force", "--yes", path}, output)) {
    return false;
  }

  int block_size = 0;
  qint64 free_blocks = 0;
  qint64 total_blocks = 0;
  for (const QString& line : output.split('\n')) {
    if (line.startsWith("blksize:")) {
      block_size = line.split(':').at(1).trimmed().toInt();
    } else if (line.startsWith("blocks:")) {
      total_blocks = line.split(':').at(1).trimmed().toLongLong();
    } else if (line.startsWith("free blocks:")) {
      free_blocks = line.split(':').at(1).trimmed().toLongLong();
    }
  }

  if (free_blocks > 0 && total_blocks > 0 && block_size > 0) {
    total = total_blocks * block_size;
    freespace = free_blocks * block_size;
    return true;
  } else {
    return false;
  }
}

bool ReadReiserfsUsage(const QString& path, qint64& freespace, qint64& total) {
  QString output;
  if (!SpawnCmd("debugreiserfs", {"-d", path}, output)) {
    return false;
  }

  qint64 total_blocks = 0;
  qint64 free_blocks = 0;
  int block_size = 0;
  for (const QString& line : output.split('\n')) {
    if (line.startsWith("Count of blocks on the device:")) {
      total_blocks = line.split(':').last().trimmed().toLongLong();
    } else if (line.startsWith("Blocksize:")) {
      block_size = line.split(':').last().trimmed().toInt();
    } else if (line.startsWith("Free blocks (count of blocks")) {
      free_blocks = line.split(':').last().trimmed().toLongLong();
    }
  }

  if (free_blocks > 0 && total_blocks > 0 && block_size > 0) {
    freespace = free_blocks * block_size;
    total = total_blocks * block_size;
    return true;
  } else {
    return false;
  }
}

bool ReadXfsUsage(const QString& path, qint64& freespace, qint64& total) {
  QString output;
  if (!SpawnCmd("xfs_db", {"-c sb", "-c print", "-r", path}, output)) {
    return false;
  }
  if (output.isEmpty()) {
    return false;
  }

  int block_size = 0;
  qint64 total_blocks = 0;
  qint64 free_blocks = 0;
  for (const QString& line : output.split('\n')) {
    if (line.contains("fdblocks")) {
      free_blocks = line.split('=').last().trimmed().toLongLong();
    } else if (line.contains("dblocks")) {
      total_blocks = line.split('=').last().trimmed().toLongLong();
    } else if (line.contains("blocksize")) {
      block_size = line.split('=').last().trimmed().toInt();
    }
  }

  if (free_blocks > 0 && total_blocks > 0 && block_size > 0) {
    freespace = free_blocks * block_size;
    total = total_blocks * block_size;
    return true;
  } else {
    return false;
  }
}

bool ReadUsage(const QString& partition_path,
               FsType fs_type,
               qint64& freespace,
               qint64& total) {
  bool ok = false;
  switch (fs_type) {
    case FsType::Btrfs: {
      ok = ReadBtrfsUsage(partition_path, freespace, total);
      break;
    }
    case FsType::Ext2:
    case FsType::Ext3:
    case FsType::Ext4: {
      ok = ReadExt2Usage(partition_path, freespace, total);
      break;
    }
    case FsType::EFI:
    case FsType::Fat16:
    case FsType::Fat32: {
      ok = ReadFat16Usage(partition_path, freespace, total);
      break;
    }
    case FsType::Hfs:
    case FsType::HfsPlus: {
      break;
    }
    case FsType::Jfs: {
      ok = ReadJfsUsage(partition_path, freespace, total);
      break;
    }
    case FsType::LinuxSwap: {
      ok = ReadLinuxSwapUsage(partition_path, freespace, total);
      break;
    }
    case FsType::Nilfs2: {
      ok = ReadNilfs2Usage(partition_path, freespace, total);
      break;
    }
    case FsType::NTFS: {
      ok = ReadNTFSUsage(partition_path, freespace, total);
      break;
    }
    case FsType::Reiser4: {
      ok = ReadReiser4Usage(partition_path, freespace, total);
      break;
    }
    case FsType::Reiserfs: {
      ok = ReadReiserfsUsage(partition_path, freespace, total);
      break;
    }
    case FsType::Xfs: {
      ok = ReadXfsUsage(partition_path, freespace, total);
      break;
    }
    default: {
      ok = false;
      break;
    }
  }

  if (!ok) {
    freespace = -1;
    total = -1;
    qWarning() << "Failed to read usage:" << partition_path;
  }
  return ok;
}
