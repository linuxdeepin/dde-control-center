//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DeveloperModeDialog_H
#define DeveloperModeDialog_H

#include "interface/namespace.h"

#include <DAbstractDialog>
#include <DRadioButton>
#include <DSuggestButton>

#include <QDebug>
#include <QDBusArgument>
#include <QDBusMetaType>

struct TS{
    QString ts1;
    QString ts2;
};

class QFileDialog;

namespace DCC_NAMESPACE {
class CommonInfoModel;

class DeveloperModeDialog : public DTK_WIDGET_NAMESPACE::DAbstractDialog
{
    Q_OBJECT
public:
    explicit DeveloperModeDialog(QObject *parent = nullptr);
    ~DeveloperModeDialog();

public:
     void setModel(CommonInfoModel *model);
     void shutdown();

Q_SIGNALS:
     void requestDeveloperMode(bool enabled);
     void requestLogin();
     void requestCommit(QString filePathName);

protected:
     virtual void showEvent(QShowEvent *);

private Q_SLOTS:
     void setLogin();

private:
    DTK_WIDGET_NAMESPACE::DRadioButton *m_onlineBtn{nullptr};
    DTK_WIDGET_NAMESPACE::DRadioButton *m_offlineBtn{nullptr};
    DTK_WIDGET_NAMESPACE::DSuggestButton *m_nextButton{nullptr};
    CommonInfoModel *m_model{nullptr};
    bool m_enterDev{false};
    QFileDialog *m_importFile;
    QFileDialog *m_exportFile;
};
}

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




#endif // DEVELPERMODEACTIVATE_H
