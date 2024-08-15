// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef COMMON_H
#define COMMON_H

#include <QRegularExpression>

#include <DSysInfo>
#include <DGuiApplicationHelper>

#include <QString>

#include <vector>

DCORE_USE_NAMESPACE

inline constexpr double Epsion = 1e-6;
inline const QString SystemUpdateType = QStringLiteral("system_upgrade");
inline const QString AppStoreUpdateType = QStringLiteral("appstore_upgrade");
inline const QString SecurityUpdateType = QStringLiteral("security_upgrade");
inline const QString UnknownUpdateType = QStringLiteral("unknown_upgrade");

inline const DSysInfo::UosType UosType = DSysInfo::uosType();
inline const DSysInfo::UosEdition UosEdition = DSysInfo::uosEditionType();
inline const bool IsServerSystem = (DSysInfo::UosServer == UosType); // 是否是服务器版
inline const bool IsCommunitySystem = (DSysInfo::UosCommunity == UosEdition); // 是否是社区版
inline const bool IsProfessionalSystem = (DSysInfo::UosProfessional == UosEdition); // 是否是专业版
inline const bool IsHomeSystem = (DSysInfo::UosHome == UosEdition);           // 是否是个人版
inline const bool IsEducationSystem = (DSysInfo::UosEducation == UosEdition); // 是否是教育版
inline const bool IsDeepinDesktop =
        (DSysInfo::DeepinDesktop == DSysInfo::deepinType()); // 是否是Deepin桌面

inline const QString TestingChannelPackage = QStringLiteral("deepin-unstable-source");
inline const QString ServiceLink = QStringLiteral("https://insider.deepin.org");

enum UpdatesStatus {
    Default,
    Checking,
    Updated,
    UpdatesAvailable,
    Updating,
    Downloading,
    DownloadPaused,
    Downloaded,
    AutoDownloaded,
    Installing,
    UpdateSucceeded,
    UpdateFailed,
    NeedRestart,
    WaitForRecoveryBackup,
    RecoveryBackingup,
    RecoveryBackingSuccessed,
    RecoveryBackupFailed,
    RecoveryBackupFailedDiskFull,
    Inactive
};

enum UpdateErrorType {
    NoError,
    NoNetwork,
    NoSpace,
    DeependenciesBrokenError,
    DpkgInterrupted,
    UnKnown
};

enum ShowStatus { NoActive, IsSuccessed, IsFailed };

enum ClassifyUpdateType {
    Invalid = -1,
    SystemUpdate = 1,
    AppStoreUpdate,
    UnknownUpdate = 8,
    SecurityUpdate = 16
};

enum UpdateCtrlType { Start = 0, Pause };

// 备份状态
enum BackupStatus { NoBackup, Backingup, Backuped, BackupFailed };

// 原子更新结果 后期更具需求拓展
enum BackupResult { Success = 0, BackingUp = 1 };

enum UiActiveState {
    Unknown = -1,     // 未知
    Unauthorized = 0, // 未授权
    Authorized,       // 已授权
    AuthorizedLapse,  // 授权失效
    TrialAuthorized,  // 试用期已授权
    TrialExpired      // 试用期已过期
};

enum TestingChannelStatus {
    DeActive,
    NotJoined,
    WaitJoined,
    WaitToLeave,
    Joined,
};

enum CanExitTestingChannelStatus {
    CheckOk,
    Cancel,
    CheckError,
};

[[maybe_unused]] static inline ClassifyUpdateType uintToclassifyUpdateType(uint type)
{
    ClassifyUpdateType value = ClassifyUpdateType::Invalid;
    switch (type) {
    case ClassifyUpdateType::SystemUpdate:
        value = ClassifyUpdateType::SystemUpdate;
        break;
    case ClassifyUpdateType::SecurityUpdate:
        value = ClassifyUpdateType::SecurityUpdate;
        break;
    case ClassifyUpdateType::UnknownUpdate:
        value = ClassifyUpdateType::UnknownUpdate;
        break;
    default:
        value = ClassifyUpdateType::Invalid;
        break;
    }

    return value;
}

// equal : false
[[maybe_unused]] static inline bool compareDouble(const double value1, const double value2)
{
    return !((value1 - value2 >= -Epsion) && (value1 - value2 <= Epsion));
}

[[maybe_unused]] static inline QString formatCap(qulonglong cap, const int size = 1024)
{
    const static QString type[] = { "B", "KB", "MB", "GB", "TB" };

    if (cap < qulonglong(size)) {
        return QString::number(cap) + type[0];
    }
    if (cap < qulonglong(size) * size) {
        return QString::number(double(cap) / size, 'f', 2) + type[1];
    }
    if (cap < qulonglong(size) * size * size) {
        return QString::number(double(cap) / size / size, 'f', 2) + type[2];
    }
    if (cap < qulonglong(size) * size * size * size) {
        return QString::number(double(cap) / size / size / size, 'f', 2) + type[3];
    }

    return QString::number(double(cap) / size / size / size / size, 'f', 2) + type[4];
}

[[maybe_unused]] static inline std::vector<double> getNumListFromStr(const QString &str)
{
    // 筛选出字符串中的数字
    QRegularExpression rx(R"(-?[1-9]\d*\.\d*|0+.[0-9]+|-?0\.\d*[1-9]\d*|-?\d+)");
    std::vector<double> v;
    QRegularExpressionMatchIterator i = rx.globalMatch(str);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        v.push_back(match.captured(0).toDouble());
    }
    return v;
}

[[maybe_unused]] static QString htmlToCorrectColor(const QString &data)
{
    const auto colorType = Dtk::Gui::DGuiApplicationHelper::instance()->themeType();
    const QString &textColor = (colorType == Dtk::Gui::DGuiApplicationHelper::LightType) ? "rgba(0, 0, 0, 0.6)" : "rgba(255, 255, 255, 0.6)";
    const QString colorRegexPattern("(background-color:\\s*rgba?\\((\\s*\\d+\\s*),\\s*(\\s*\\d+\\s*),\\s*(\\s*\\d+\\s*)(?:,\\s*(?:\\d*\\.)?\\d+\\s*)?\\);)|(rgba?\\((\\s*\\d+\\s*),\\s*(\\s*\\d+\\s*),\\s*(\\s*\\d+\\s*)(?:,\\s*(?:\\d*\\.)?\\d+\\s*)?\\))");

    QRegularExpression regex(colorRegexPattern);

    QString result;
    result.reserve(data.size());

    int lastMatchEnd = 0;
    for (auto it = regex.globalMatch(data); it.hasNext(); ) {
        auto match = it.next();
        if (match.hasMatch() && match.captured(1).isEmpty()) { // 匹配到文字颜色，替换
            result.append(data.midRef(lastMatchEnd, match.capturedStart() - lastMatchEnd));
            result.append(textColor);
        } else { // 匹配到背景颜色，删除
            result.append(data.midRef(lastMatchEnd, match.capturedStart() - lastMatchEnd));
        }
        lastMatchEnd = match.capturedEnd();
    }
    result.append(data.midRef(lastMatchEnd));

    return result;
}

#endif // COMMON_H
