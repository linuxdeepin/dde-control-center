/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef INPUTPASSWORDDIALOG_H
#define INPUTPASSWORDDIALOG_H

#include <QWidget>
#include <QPointer>

#include <dboxwidget.h>
#include <dpasswordedit.h>
#include <dcheckbox.h>
#include <dseparatorhorizontal.h>

DWIDGET_USE_NAMESPACE

class QEventLoop;
class InputPasswordDialog : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(bool autoConnect READ autoConnect)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

public:
    explicit InputPasswordDialog(QWidget *parent = 0);
    ~InputPasswordDialog();

    bool autoConnect() const;
    QString text() const;
    void setInputAlert(bool alert);
    void setAutoConnect(bool autoConnect);

public slots:
    int exec();
    void done(int code);

protected:
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent *e) Q_DECL_OVERRIDE;

signals:
    void setText(const QString &text);

    void cancel();
    void confirm();
    void textChanged(QString text);

private slots:
    void updateMainWidgetPos();

private:
    DPasswordEdit *m_edit;
    DCheckBox *m_checkBox;
    DVBoxWidget *m_mainWidget;
    QPointer<QEventLoop> m_eventLoop;
};

#endif // INPUTPASSWORDDIALOG_H
