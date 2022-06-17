/*
* Copyright (C) 2019 ~ 2022 Deepin Technology Co., Ltd.
*
* Author:     guoyao <guoyao@uniontech.com>

* Maintainer: guoyao <guoyao@uniontech.com>

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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
    QLabel *m_hostNameTitleLabel;
    Dtk::Widget::DLabel *m_hostNameLabel;
    QToolButton *m_hostNameBtn;
    HostNameEdit *m_hostNameLineEdit;
    QString m_alertMessage;
    QString m_hostname;//保存计算机的全名
    QString m_hostnameEdit;//保存编辑时的数据
};
