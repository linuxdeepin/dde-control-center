//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef TIMEOUTDIALOG_H
#define TIMEOUTDIALOG_H

#include <QTimer>
#include <DDialog>

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

protected:
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    QTimer *m_timeoutRefreshTimer;

    int m_timeout;
    QString m_messageModel;
};

#endif // TIMEOUTDIALOG_H
