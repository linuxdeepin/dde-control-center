/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

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
        UserNameError,
        PasswordError,
        PasswordMatchError,
        UnknownError,
        NoError
    };

    explicit CreationResult(QObject *parent = 0);
    explicit CreationResult(ResultType type, QString message, QObject *parent = 0);

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
