//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QDBusInterface>
#include <QObject>


#include "pwqualitymanager.h"

#include <QDBusArgument>

class QProcess;
class CommonInfoProxy;

class CommonInfoModel;

struct DebugArg {
    QString module;
    QString state;
};
Q_DECLARE_METATYPE(DebugArg)
typedef QList<DebugArg> DebugArgList;
Q_DECLARE_METATYPE(DebugArgList)


class CommonInfoWork : public QObject
{
    Q_OBJECT
public:
    explicit CommonInfoWork(CommonInfoModel *model, QObject *parent = nullptr);
    virtual ~CommonInfoWork();

    void active();
    bool isSettingPlymouth() { return m_scaleIsSetting; }

    void initGrubAnimationModel();
    void initGrubMenuListModel();
    void initDebugLogLevel();

    Q_INVOKABLE QString verifyPassword(QString text);
    Q_INVOKABLE void jumpToSecurityCenter();
    Q_INVOKABLE void setLogDebug(int index);
    Q_INVOKABLE void importCertificate(QString filePath);
    Q_INVOKABLE void exportMessage(QString filePath);
    Q_INVOKABLE void setBackground(const QString &path);
    Q_INVOKABLE bool isSecurityCenterInstalled();

public Q_SLOTS:
    void setBootDelay(bool value);
    void setEnableTheme(bool value);
    void setDefaultEntry(const QString &entry);
    void disableGrubEditAuth();
    void onSetGrubEditPasswd(const QString &password, const bool &isReset);
    void setUeProgram(bool enabled);
    void closeUeProgram();
    void setEnableDeveloperMode(bool enabled);
    void login();
    void deepinIdErrorSlot(int code, const QString &msg);
    void setPlymouthFactor(int factor);
    void resetEditAuthEnabled();

Q_SIGNALS:
    void settingScaling(bool);

private:
    QString passwdEncrypt(const QString &password);
    std::pair<int, QString> getPlyMouthInformation();

private:
    CommonInfoModel *m_commomModel;
    CommonInfoProxy *m_commonInfoProxy;
    QProcess *m_process = nullptr;
    QString m_title;
    QString m_content;
    bool m_scaleIsSetting;
    QDBusInterface *m_debugConfigInter;
    QDBusInterface *m_inter;
};


class DMIInfo
{
public:
    DMIInfo(){}

    friend QDebug operator<<(QDebug debug, const DMIInfo &info)
    {
        debug << QString("DMIInfo(") << info.biosVendor << ", " << info.biosVersion << ", "
              << info.biosDate << ", " << info.boardName << ", " << info.boardSerial << ", "
              << info.boardVendor << ", " << info.boardVersion << ", "
              << info.productName << ", " << info.productFamily << ", "
              << info.producctSerial << ", " << info.productUUID << ", "
              << info.productVersion << ")";

        return debug;
    }
    friend const QDBusArgument &operator>>(const QDBusArgument &arg, DMIInfo &info)
    {
        arg.beginStructure();
        arg >> info.biosVendor >> info.biosVersion >> info.biosDate
                >> info.boardName >> info.boardSerial >> info.boardVendor >> info.boardVersion
                >> info.productName >> info.productFamily
                >> info.producctSerial >> info.productUUID >> info.productVersion;
        arg.endStructure();

        return arg;
    }
    friend QDBusArgument &operator<<(QDBusArgument &arg, const DMIInfo &info)
    {
        arg.beginStructure();
        arg << info.biosVendor << info.biosVersion << info.biosDate
            << info.boardName << info.boardSerial << info.boardVendor << info.boardVersion
            << info.productName << info.productFamily
            << info.producctSerial << info.productUUID << info.productVersion;
        arg.endStructure();

        return arg;
    }

public:
    QString biosVendor{""};
    QString biosVersion{""};
    QString biosDate{""};
    QString boardName{""};
    QString boardSerial{""};
    QString boardVendor{""};
    QString boardVersion{""};
    QString productName{""};
    QString productFamily{""};
    QString producctSerial{""};
    QString productUUID{""};
    QString productVersion{""};
};

class HardwareInfo
{
public:
    HardwareInfo(){}

    friend QDebug operator<<(QDebug debug, const HardwareInfo &info)
    {
        debug << "HardwareInfo(" <<info.id << ", " << info.hostName  << ", "
              << info.username  << ", " << info.os  << ", " << info.cpu << ", "
              << info.laptop  << ", " << info.memory  << ", " << info.diskTotal
              << ", " << info.networkCards << ", " << info.diskList  << ", " << info.dmi << ')';

        return debug;
    }
    friend QDBusArgument &operator<<(QDBusArgument &arg, const HardwareInfo &info)
    {
        arg.beginStructure();
        arg << info.id << info.hostName << info.username << info.os << info.cpu
            << info.laptop << info.memory << info.diskTotal << info.networkCards
            << info.diskList << info.dmi;
        arg.endStructure();

        return arg;
    }
    friend const QDBusArgument &operator>>(const QDBusArgument &arg, HardwareInfo &info)
    {
        arg.beginStructure();
        arg >> info.id >> info.hostName >> info.username >> info.os >> info.cpu
                >> info.laptop >> info.memory >> info.diskTotal >> info.networkCards
                >> info.diskList >> info.dmi;
        arg.endStructure();

        return arg;
    }

public:
    QString id{""};
    QString hostName{""};
    QString username{""};
    QString os{""};
    QString cpu{""};
    bool laptop{false};
    qint64 memory{0};
    qint64 diskTotal{0};
    QString networkCards{""};
    QString diskList{""};
    DMIInfo dmi;
};

Q_DECLARE_METATYPE(DMIInfo)
Q_DECLARE_METATYPE(HardwareInfo)

