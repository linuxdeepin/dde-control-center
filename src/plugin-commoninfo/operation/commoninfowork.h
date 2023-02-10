//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "interface/namespace.h"

#include <QObject>

class QProcess;
class CommonInfoProxy;

namespace DCC_NAMESPACE {

class CommonInfoModel;

class CommonInfoWork : public QObject
{
    Q_OBJECT
public:
    explicit CommonInfoWork(CommonInfoModel *model, QObject *parent = nullptr);
    virtual ~CommonInfoWork();

    void active();

public Q_SLOTS:
    void setBootDelay(bool value);
    void setEnableTheme(bool value);
    void setDefaultEntry(const QString &entry);
    void disableGrubEditAuth();
    void onSetGrubEditPasswd(const QString &password, const bool &isReset);
    void setBackground(const QString &path);
    void setUeProgram(bool enabled);
    void closeUeProgram();
    void setEnableDeveloperMode(bool enabled);
    void login();
    void deepinIdErrorSlot(int code, const QString &msg);

private:
    QString passwdEncrypt(const QString &password);

private:
    CommonInfoModel *m_commomModel;
    CommonInfoProxy *m_commonInfoProxy;
    QProcess *m_process = nullptr;
    QString m_title;
    QString m_content;
};
} // namespace DCC_NAMESPACE
