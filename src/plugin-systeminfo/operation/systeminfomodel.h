//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SYSTEMINFOMODEL_H
#define SYSTEMINFOMODEL_H

#include "qcolor.h"

#include <dsysinfo.h>

#include <QObject>

#include <optional>

using namespace Dtk::Core;

namespace DCC_NAMESPACE{

enum ActiveState {
    Unknown = -1,  //未知
    Unauthorized = 0,  //未授权
    Authorized,  //已授权
    AuthorizedLapse,  //授权失效
    TrialAuthorized, //试用期已授权
    TrialExpired //试用期已过期
};

// !!! 不要用C++11的前置声明枚举类型，这里lupdate命令有个bug．具体见
// https://stackoverflow.com/questions/6504902/lupdate-error-qualifying-with-unknown-namespace-class

class SystemInfoModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString privacyPolicy READ privacyPolicy NOTIFY privacyPolicyChanged FINAL)
    Q_PROPERTY(QString gnuLinceseTitle READ gnuLinceseTitle NOTIFY gnuLinceseTitleChanged FINAL)
    Q_PROPERTY(QString gnuLinceseContent READ gnuLinceseContent NOTIFY gnuLinceseContentChanged FINAL)
    Q_PROPERTY(QString userLicense READ userLicense NOTIFY userLicenseChanged FINAL)
    Q_PROPERTY(QString userExperienceProgramText READ userExperienceProgramText NOTIFY userExperienceProgramTextChanged FINAL)
    Q_PROPERTY(bool joinUeProgram READ joinUeProgram NOTIFY joinUeProgramChanged FINAL)

    Q_PROPERTY(QString productName READ productName NOTIFY productNameChanged FINAL)
    Q_PROPERTY(QString systemCopyright READ systemCopyright NOTIFY systemCopyrightChanged FINAL)
    Q_PROPERTY(QString hostName READ hostName NOTIFY hostNameChanged FINAL)
    Q_PROPERTY(QString version READ version NOTIFY versionChanged FINAL)
    Q_PROPERTY(QString versionNumber READ versionNumber NOTIFY versionNumberChanged FINAL)
    Q_PROPERTY(QString type READ type NOTIFY typeChanged FINAL)
    Q_PROPERTY(QString memory READ memory NOTIFY memoryChanged FINAL)
    Q_PROPERTY(QString kernel READ kernel NOTIFY kernelChanged FINAL)
    Q_PROPERTY(QString processor READ processor NOTIFY processorChanged FINAL)
    Q_PROPERTY(QString licenseStatusText READ licenseStatusText NOTIFY licenseStatusTextChanged FINAL)
    Q_PROPERTY(QString licenseActionText READ licenseActionText NOTIFY licenseActionTextChanged FINAL)
    Q_PROPERTY(QColor licenseStatusColor READ licenseStatusColor NOTIFY licenseStatusColorChanged FINAL)
    Q_PROPERTY(bool showDetail READ showDetail NOTIFY showDetailChanged FINAL)
    Q_PROPERTY(QString logoPath READ logoPath NOTIFY logoPathChanged FINAL)
    Q_PROPERTY(QString systemInstallationDate READ systemInstallationDate NOTIFY systemInstallationDateChanged FINAL)
    Q_PROPERTY(QString graphicsPlatform READ graphicsPlatform NOTIFY graphicsPlatformChanged FINAL)

public:
    explicit SystemInfoModel(QObject *parent = nullptr);

    QString productName() const { return m_productName;}
    QString versionNumber() const { return m_versionNumber;}
    QString version() const { return m_version;}
    QString type() const { return m_type;}
    QString processor() const { return m_processor;}
    QString memory() const { return m_memory;}
    QString kernel() const { return m_kernel;}
    QString hostName() const { return  m_hostName;}

    inline std::optional<QString> endUserAgreementPath() const { return m_endUserAgreementTextPath; }
    inline ActiveState licenseState() const { return m_licenseState; }

    QString privacyPolicy() const;
    void setPrivacyPolicy(const QString &newPrivacyPolicy);

    QString gnuLinceseTitle() const;
    void setGnuLinceseTitle(const QString &newGnuLinceseTitle);

    QString gnuLinceseContent() const;
    void setGnuLinceseContent(const QString &newGnuLinceseContent);

    QString userLicense() const;
    void setUserLicense(const QString &newUserLicense);

    QString userExperienceProgramText() const;
    void setUserExperienceProgramText(const QString &newUserExperienceProgramText);

    bool joinUeProgram() const;
    void setJoinUeProgram(bool newJoinUeProgram);

    QString systemCopyright() const;
    void setSystemCopyright(const QString &newSystemCopyright);

    QString licenseStatusText() const;
    void setLicenseStatusText(const QString &newLicenseStatusText);

    QString licenseActionText() const;
    void setLicenseActionText(const QString &newLicenseActionText);

    QColor licenseStatusColor() const;
    void setLicenseStatusColor(const QColor &newLicenseStatusColor);

    void onLicenseStateChanged(const ActiveState &state);

    bool showDetail() const;
    void setShowDetail(bool newShowDetail);

    QString logoPath() const;
    void setLogoPath(const QString &newLogoPath);

    QString systemInstallationDate() const;
    void setSystemInstallationDate(const QString &newSystemInstallationDate);

    Q_INVOKABLE bool showAuthorization() const;
    Q_INVOKABLE bool showUserExperienceProgram() const;
    Q_INVOKABLE bool showGraphicsPlatform() const;

    QString graphicsPlatform() const;
    void setGraphicsPlatform(const QString &newGraphicsPlatform);

Q_SIGNALS:
    void productNameChanged(const QString& version);
    void versionNumberChanged(const QString& version);
    void versionChanged(const QString& version);
    void typeChanged(const QString& type);
    void processorChanged(const QString& processor);
    void memoryChanged(const QString& memory);
    void kernelChanged(const QString& kernel);
    void licenseStateChanged(ActiveState state);
    void hostNameChanged(const QString& hostName);
    void setHostNameError(const QString& error);

    void privacyPolicyChanged();

    void gnuLinceseTitleChanged();
    void gnuLinceseContentChanged();

    void userLicenseChanged();

    void userExperienceProgramTextChanged();

    void joinUeProgramChanged(const bool enable) const;

    void systemCopyrightChanged();

    void licenseStateDataChanged();

    void licenseStatusTextChanged();

    void licenseActionTextChanged();

    void licenseStatusColorChanged();


    void showDetailChanged();

    void logoPathChanged();

    void systemInstallationDateChanged();

    void graphicsPlatformChanged();

public Q_SLOTS:
    void setProductName(const QString& name);
    void setVersionNumber(const QString& number);
    void setVersion(const QString& version);
    void setType(qlonglong type);
    void setProcessor(const QString& processor);
    void setMemory(qulonglong totalMemory, qulonglong installedMemory);
    void setKernel(const QString &kernel);
    void setLicenseState(DCC_NAMESPACE::ActiveState state);
    void setHostName(const QString& hostName);
    void setEndUserAgreementPath(const QString &path);

private:
    QString m_version;
    QString m_productName;
    QString m_versionNumber;
    QString m_type;
    QString m_processor;
    QString m_memory;
    QString m_kernel;
    QString m_hostName;
    std::optional<QString> m_endUserAgreementTextPath;
    DCC_NAMESPACE::ActiveState m_licenseState;

    // 隐私协议
    QString m_privacyPolicy;
    // 开源协议
    QString m_gnuLinceseTitle;
    QString m_gnuLinceseContent;
    // 最终用户协议
    QString m_userLicense;
    // 用户体验计划文本
    QString m_userExperienceProgramText;
    bool m_joinUeProgram;

    QString m_systemCopyright;
    QString m_licenseStatusText;
    QColor m_licenseStatusColor;
    QString m_licenseActionText;

    QString m_logoPath;

    bool m_showDetail;
    QString m_systemInstallationDate;
    QString m_graphicsPlatform;
};

}
Q_DECLARE_METATYPE(DCC_NAMESPACE::ActiveState);
#endif // SYSTEMINFOMODEL_H
