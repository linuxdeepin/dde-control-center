/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TIMEOUTDIALOG_H
#define TIMEOUTDIALOG_H

#include <QTimer>
#include <ddialog.h>

DWIDGET_USE_NAMESPACE

class TimeoutDialog : public DDialog
{
    Q_OBJECT
public:
    explicit TimeoutDialog(const int timeout, QString messageModel = QString(), QWidget *parent = 0);

    QString messageModel() const;
    void setMessageModel(const QString &messageModel);

public Q_SLOTS:
    int exec() override;
    void open() override;

private Q_SLOTS:
    void onRefreshTimeout();

private:
    QTimer *m_timeoutRefreshTimer;

    int m_timeout;
    QString m_messageModel;
};

#endif // TIMEOUTDIALOG_H
