// Copyright (c) 2016 Deepin Ltd. All rights reserved.
// Use of this source is governed by General Public License that can be found
// in the LICENSE file.

#ifndef INSTALLER_SETTINGS_MANAGER_H
#define INSTALLER_SETTINGS_MANAGER_H

#include <QString>
#include <QStringList>
#include <QVariant>

namespace installer {

// Read settings value from ini file.

// Get boolean option value from settings file.
// If |key| does not exist, returns false.
bool GetSettingsBool(const QString& key);

// Get integer option value from settings file.
// If |key| does not exist, return 0.
int GetSettingsInt(const QString& key);

// Get string option value from settings file.
// If |key| does not exist, returns an empty string.
QString GetSettingsString(const QString& key);

// Get string list option value form settings file.
// Items in value are separated by ;
// If |key| does not exist, returns an empty string list.
QStringList GetSettingsStringList(const QString& key);

// Get variant option value from settings file.
QVariant GetSettingsValue(const QString& key);

// Returns absolute path to oem/auto_part.sh
QString GetAutoPartFile();

// Get all available avatars in avatar folder.
QStringList GetAvatars();

// Get default user avatar, is specified. Or else returns a random avatar.
QString GetDefaultAvatar();

// Get absolute path to oem hooks folder.
QString GetOemHooksDir();

// Get vendor logo.
QString GetVendorLogo();

// Get image path of installer window background.
// First try to find installer background image in oem folder.
// If not found, use the fallback image.
QString GetWindowBackground();

// Append settings in |conf_file| into default conf file.
bool AppendConfigFile(const QString& conf_file);

// Operations of /etc/deepin-installer.conf
bool DeleteConfigFile();

// Setup uefi mode or not.
void WriteUEFI(bool is_efi);
//void WriteInstallerMode(bool is_simple_mode);

// Get current locale.
QString ReadLocale();
void WriteAvatar(const QString& avatar);
void WriteFoundWindowsPartition(bool found);
void WriteHostname(const QString& hostname);
// Write keyboard model, layout and layout variant name.
void WriteKeyboard(const QString& model,
                   const QString& layout,
                   const QString& variant);
void WriteLocale(const QString& locale);
void WritePassword(const QString& password);
void WriteTimezone(const QString& timezone, bool is_local_time);
void WriteUsername(const QString& username);

// Write disk info.
//  * |root_disk|, device path to install system into, like /dev/sda;
//  * |root_partition|, partition path to install system into;
//  * |boot_partition|, partition path to install grub into;
//  * |mount_point|, a list of partition path and mount-point info,
//    items are separated by ';'
void WritePartitionInfo(const QString& root_disk,
                        const QString& root_partition,
                        const QString& boot_partition,
                        const QString& mount_points);

// Whether swap file is required. Swap file is created in before_chroot/.
void WriteRequiringSwapFile(bool is_required);

// Save current settings to /etc/deepin-installer.conf
// Other settings will be updated later.
void AddConfigFile();

}  // namespace installer

#endif  // INSTALLER_SETTINGS_MANAGER_H
