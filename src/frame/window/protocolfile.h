// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QString>
#include <QObject>

class ProtocolFile : public QObject
{
    Q_OBJECT

public:
    // 获取隐私协议
    static void getPrivacyFile(QString &zhCN_Content, QString &enUS_Content);
    // 获取用户体验计划许可协议
    static QString getUserExpContent();
    // 最终用户许可协议
    static QString getEnduserAgreement();
    // 欧拉最终用户许可协议
    static QString getEulerEnduserAgreement();
};
