// Copyright (c) 2016 Deepin Ltd. All rights reserved.
// Use of this source is governed by General Public License that can be found
// in the LICENSE file.

#include "settings_manager.h"

#include <fcntl.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfoList>
#include <QHash>
#include <QSettings>

#include "consts.h"

namespace installer {

namespace {

// Absolute path to oem dir.
QString g_oem_dir;

const char kLocaleKey[] = "DI_LOCALE";

// Absolute path to installer config file.
const char kInstallerConfigFile[] = "/etc/deepin-installer.conf";

// File name of installer wallpaper.
const char kOemWallpaperFilename[] = "installer-background.jpg";

// File name of auto partition script.
const char kAutoPartFile[] = "auto_part.sh";
// File name of architecture specific of auto partition script.
const char kAutoPartArchSpecFile[] = "auto_part_%1.sh";

// Absolute path to oem folder.
const char kDebugOemDir[] = "/tmp/oem";
const char kUbuntuOemDir[] = "/cdrom/oem";
const char kDeepinOemDir[] = "/lib/live/mount/medium/oem";

// Filename of oem settings
const char kOemSettingsFilename[] = "settings.ini";

void AppendToConfigFile(const QString& key, const QVariant& value) {
  QSettings settings(kInstallerConfigFile, QSettings::IniFormat);
  settings.setValue(key, value);
}

QStringList ListImageFiles(const QString& dir_name) {
  QStringList result;
  QDir dir(dir_name);
  if (!dir.exists()) {
    return result;
  }

  const QStringList name_filters = { "*.png", "*.jpg" };
  const QFileInfoList info_list =
      dir.entryInfoList(name_filters, QDir::NoDotAndDotDot | QDir::Files);
  for (const QFileInfo& info : info_list) {
    if (info.size() > 0) {
      result.append(info.absoluteFilePath());
    }
  }

  return result;
}

// Note that oem folder might not exist.
QDir GetOemDir() {
  if (g_oem_dir.isEmpty()) {
    if (QDir(kDebugOemDir).exists()) {
      g_oem_dir = kDebugOemDir;
    } else if (QDir(kUbuntuOemDir).exists()) {
      g_oem_dir = kUbuntuOemDir;
    } else {
      g_oem_dir = kDeepinOemDir;
    }
  }
  return QDir(g_oem_dir);
}

}  // namespace

bool GetSettingsBool(const QString& key) {
  const QVariant value = GetSettingsValue(key);
  if (value.isValid()) {
    return value.toBool();
  }

  qCritical() << "GetSettingsBool() failed with key:" << key;
  return false;
}

int GetSettingsInt(const QString& key) {
  const QVariant value = GetSettingsValue(key);
  if (value.isValid()) {
    return value.toInt();
  }

  qCritical() << "GetSettingsInt() failed with key:" << key;
  return 0;
}

QString GetSettingsString(const QString& key) {
  const QVariant value = GetSettingsValue(key);
  if (value.isValid()) {
    return value.toString();
  }

  qCritical() << "GetSettingsString() failed with key:" << key;
  return QString();
}

QStringList GetSettingsStringList(const QString& key) {
  const QVariant value = GetSettingsValue(key);
  if (!value.isValid()) {
    qCritical() << "GetSettingsStringList() failed with key:" << key;
    return QStringList();
  }

  return value.toString().split(';');
}

QVariant GetSettingsValue(const QString& key) {
  QSettings settings(kInstallerConfigFile, QSettings::IniFormat);
  const QVariant value(settings.value(key));
  if (!value.isValid()) {
    qWarning() << "getSettingsValue() Invalid key:" << key;
  }
  return value;
}

QString GetAutoPartFile() {
  const QString arch_spec_file =
      QString(kAutoPartArchSpecFile).arg(GetMachineArchName());
  QDir oem_dir = GetOemDir();
  QDir builtin_dir(BUILTIN_HOOKS_DIR);

  // First check existence of architecture specific file.
  const QStringList script_files = {
      oem_dir.absoluteFilePath(arch_spec_file),
      builtin_dir.absoluteFilePath(arch_spec_file),
      oem_dir.absoluteFilePath(kAutoPartFile),
      builtin_dir.absoluteFilePath(kAutoPartFile),
  };
  for (const QString filepath : script_files) {
    if (QFile::exists(filepath)) {
      return filepath;
    }
  }

  qCritical() << "GetAutoPartFile() not partition script found!";
  return QString();
}

QStringList GetAvatars() {
  // First, check oem/ dir.
  const QString oem_avatar(GetOemDir().absoluteFilePath("avatar"));
  QStringList avatars = ListImageFiles(oem_avatar);
  if (!avatars.isEmpty()) {
    return avatars;
  }

  // Then, check dde-account-faces dir.
  return ListImageFiles(GetSettingsString(kSystemInfoDdeAvatarDir));
}

QString GetDefaultAvatar() {
  const QString default_avatar(GetSettingsString(kSystemInfoDefaultAvator));
  if (!default_avatar.isEmpty() && QFile::exists(default_avatar)) {
    // Returns default avatar
    return default_avatar;
  }

  // Pick a random avatar.
  const int num = static_cast<int>(time(NULL));
  const QStringList avatars = GetAvatars();
  if (avatars.isEmpty()) {
    return "";
  }
  const int index = num % avatars.length();
  return avatars.at(index);
}

QString GetOemHooksDir() {
  return GetOemDir().absoluteFilePath("hooks");
}

QString GetVendorLogo() {
  const QString oem_file = GetOemDir().absoluteFilePath("logo.png");
  if (QFile::exists(oem_file)) {
    return oem_file;
  }

  // Returns builtin logo.
  return ":/images/logo.png";
}

QString GetWindowBackground() {
  const QString oem_file = GetOemDir().absoluteFilePath(kOemWallpaperFilename);
  if (QFile::exists(oem_file)) {
    return oem_file;
  }

  return kDefaultWallpaperFile;
}

bool AppendConfigFile(const QString& conf_file) {
  if (!QFile::exists(conf_file)) {
    qCritical() << "conf_file not found:" << conf_file;
    return false;
  }

  QSettings target_settings(kInstallerConfigFile, QSettings::IniFormat);
  QSettings new_settings(conf_file, QSettings::IniFormat);

  for (const QString& key : new_settings.allKeys()) {
    const QVariant value = new_settings.value(key);
    target_settings.setValue(key, value);
  }

  return true;
}

bool DeleteConfigFile() {
  QFile file(kInstallerConfigFile);
  if (file.exists()) {
    if (!file.remove()) {
      qCritical() << "Failed to delete installer config file!";
      return false;
    }
  }
  return true;
}

void WriteUEFI(bool is_efi) {
  AppendToConfigFile("DI_UEFI", is_efi);
}

QString ReadLocale() {
  QSettings settings(kInstallerConfigFile, QSettings::IniFormat);
  QString locale;

  // Get user-selected locale.
  locale = settings.value(kLocaleKey).toString();
  if (!locale.isEmpty()) {
    return locale;
  }

  // Get default locale in settings.ini.
  locale = settings.value(kSelectLanguageDefaultLocale).toString();
  if (!locale.isEmpty()) {
    return locale;
  }

  // Get fallback locale.
  return kDefaultLocale;
}

void WriteAvatar(const QString& avatar) {
  AppendToConfigFile("DI_AVATAR", avatar);
}

void WriteFoundWindowsPartition(bool found) {
  AppendToConfigFile(kSystemInfoFoundWindowsSystem, found);
}

void WriteHostname(const QString& hostname) {
  AppendToConfigFile("DI_HOSTNAME", hostname);
}

void WriteKeyboard(const QString& model,
                   const QString& layout,
                   const QString& variant) {
  QSettings settings(kInstallerConfigFile, QSettings::IniFormat);
  settings.setValue("DI_KEYBOARD_MODEL", model);
  settings.setValue("DI_KEYBOARD_LAYOUT", layout);
  settings.setValue("DI_KEYBOARD_LAYOUT_VARIANT", variant);
}

void WriteLocale(const QString& locale) {
  AppendToConfigFile("DI_LOCALE", locale);
}

void WritePassword(const QString& password) {
  const QString encoded_password = password.toUtf8().toBase64();
  AppendToConfigFile("DI_PASSWORD", encoded_password);
}

void WriteTimezone(const QString& timezone, bool is_local_time) {
  QSettings settings(kInstallerConfigFile, QSettings::IniFormat);
  settings.setValue("DI_TIMEZONE", timezone);
  settings.setValue("DI_IS_LOCAL_TIME", is_local_time);
}

void WriteUsername(const QString& username) {
  AppendToConfigFile("DI_USERNAME", username);
}

void WritePartitionInfo(const QString& root_disk,
                        const QString& root_partition,
                        const QString& boot_partition,
                        const QString& mount_points) {
  QSettings settings(kInstallerConfigFile, QSettings::IniFormat);
  settings.setValue("DI_ROOT_DISK", root_disk);
  settings.setValue("DI_ROOT_PARTITION", root_partition);
  settings.setValue("DI_BOOTLOADER", boot_partition);
  settings.setValue("DI_MOUNTPOINTS", mount_points);
}

void WriteRequiringSwapFile(bool is_required) {
  AppendToConfigFile("DI_SWAP_FILE_REQUIRED", is_required);
}

void AddConfigFile() {
  QSettings target_settings(kInstallerConfigFile, QSettings::IniFormat);

  // Read default settings
  QSettings default_settings(kDefaultSettingsFile, QSettings::IniFormat);
  for (const QString& key : default_settings.allKeys()) {
    const QVariant value(default_settings.value(key));
    // Do not use section groups.
    target_settings.setValue(key, value);
  }

  // Read oem settings
  const QString oem_file = GetOemDir().absoluteFilePath(kOemSettingsFilename);
  if (QFile::exists(oem_file)) {
    QSettings oem_settings(oem_file , QSettings::IniFormat);
    for (const QString& key : oem_settings.allKeys()) {
      const QVariant value = oem_settings.value(key);
      target_settings.setValue(key, value);
    }
  }
}

}  // namespace installer
