// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DCC_ACCOUNTSS_CREATIONRESULT_H
#define DCC_ACCOUNTSS_CREATIONRESULT_H

#include <QObject>

namespace dcc {
namespace accounts {

class CreationResult : public QObject
{
    Q_OBJECT
public:
    enum ResultType {
        UserNameError,      // 用户名错误
        PasswordError,      // 密码错误
        PasswordMatchError, // 两次输入的密码不匹配
        UnknownError,       // 未知错误
        Canceled,           // 用户取消认证或关闭认证窗口
        NoError
    };

    explicit CreationResult(QObject *parent = 0);
    explicit CreationResult(ResultType type, const QString &message, QObject *parent = 0);

    inline ResultType type() const { return m_type; }
    void setType(const ResultType &type);

    inline QString message() const { return m_message; }
    void setMessage(const QString &message);

private:
    ResultType m_type;
    QString m_message;
};

} // namespace accounts
} // namespace dcc

#endif // DCC_ACCOUNTSS_CREATIONRESULT_H
