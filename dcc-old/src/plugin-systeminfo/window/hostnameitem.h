// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <widgets/settingsitem.h>

#include <DLineEdit>

DWIDGET_BEGIN_NAMESPACE
class DLabel;
DWIDGET_END_NAMESPACE
class QLabel;
class QToolButton;

class HostNameEdit : public DTK_WIDGET_NAMESPACE::DLineEdit
{
    Q_OBJECT
public:
    explicit HostNameEdit(QWidget *parent = nullptr);

protected:
    bool eventFilter(QObject *, QEvent *) override;
};

class HostNameItem : public DCC_NAMESPACE::SettingsItem
{
    Q_OBJECT
public:
    explicit HostNameItem(QWidget *parent = nullptr);

    void setHostName(const QString &name);
    void onSetError(const QString &error);

Q_SIGNALS:
    void hostNameChanged(const QString &name);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void initUI();
    QString getElidedText(const QString &string);

    void onToolButtonButtonClicked();
    void onFocusChanged(const bool onFocus);
    void onTextEdited(const QString &hostName);
    void onAlertChanged();
    void onEditingFinished();

private:
    QLabel *m_computerLabel;
    Dtk::Widget::DLabel *m_hostNameLabel;
    QToolButton *m_hostNameBtn;
    HostNameEdit *m_hostNameLineEdit;
    QString m_alertMessage;
    QString m_hostname;     // 保存计算机的全名
    QString m_hostnameEdit; // 保存编辑时的数
    QString m_iconName;     //
};
