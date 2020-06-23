#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QDebug>

enum class FsType {
  Empty,
  Btrfs,
  EFI,
  Ext2,
  Ext3,
  Ext4,
  F2fs,
  Fat16,
  Fat32,
  Hfs,
  HfsPlus,
  Jfs,
  LinuxSwap,
  LVM2PV,
  Nilfs2,
  NTFS,
  Others,
  Reiser4,
  Reiserfs,
  Xfs,
  Recovery,
  Unknown,
};
QDebug& operator<<(QDebug& debug, const FsType& fs_type);

// Convert between FsType enum and its name.
FsType GetFsTypeByName(const QString& name);
QString GetFsTypeName(FsType fs_type);

typedef QList<FsType> FsTypeList;

#endif // FILESYSTEM_H
