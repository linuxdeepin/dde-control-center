/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef ACCOUNTNAMETITLE_H
#define ACCOUNTNAMETITLE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class HeaderNameLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(bool isCurrentUser READ isCurrentUser WRITE setIsCurrentUser)
public:
    explicit HeaderNameLabel(QWidget * = 0);

    void setIsCurrentUser(bool value);
    bool isCurrentUser() const;

private:
    bool m_isCurrentUser = false;
};

class UserNameTitle : public QWidget
{
    Q_OBJECT
public:
    explicit UserNameTitle(QWidget *parent = 0);
    void setUserName(const QString &name);
    void setUserType(const QString &type);
    void setIsCurrentUser(bool v);

private:
    QVBoxLayout *m_mainLayout = NULL;
    HeaderNameLabel *m_nameLabel = NULL;
    QLabel *m_typeLabel = NULL;
};

#endif // ACCOUNTNAMETITLE_H
