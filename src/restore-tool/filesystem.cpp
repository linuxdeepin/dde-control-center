#include "filesystem.h"

static const QMap<FsType, QString> FS_TYPE_MAP{
    { FsType::Empty, "" },
    { FsType::Btrfs, "btrfs" },
    { FsType::EFI, "efi" },
    { FsType::Ext2, "ext2" },
    { FsType::Ext3, "ext3" },
    { FsType::Ext4, "ext4" },
    { FsType::F2fs, "f2fs" },
    { FsType::Fat16, "fat16" },
    { FsType::Fat32, "fat32" },
    { FsType::Hfs, "hfs" },
    { FsType::HfsPlus, "hfsplus" },
    { FsType::Jfs, "jfs" },
    { FsType::NTFS, "ntfs" },
    { FsType::Nilfs2, "nilfs2" },
    { FsType::LVM2PV, "lvm2 pv" },
    { FsType::Reiser4, "reiser4" },
    { FsType::Reiserfs, "reiserfs" },
    { FsType::Unknown, "unknown" },
    { FsType::Xfs, "xfs" },
    { FsType::Recovery, "recovery" }
};

// Unused file system.
const char kFsUnused[] = "unused";

QDebug& operator<<(QDebug& debug, const FsType& fs_type) {
  debug << GetFsTypeName(fs_type);
  return debug;
}

FsType GetFsTypeByName(const QString& name) {
    const QString lower = name.toLower();

    if (lower.isEmpty() || lower == kFsUnused) return FsType::Empty;
    if (lower.startsWith("linux-swap")) return FsType::LinuxSwap;

    return FS_TYPE_MAP.key(lower, FsType::Unknown);
}

QString GetFsTypeName(FsType fs_type) {
    switch (fs_type) {
        case FsType::LinuxSwap: {
            return "linux-swap";
        }
        default: {
            return FS_TYPE_MAP.value(fs_type);
        }
    }
}
