/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
 *             Tianlu Shao <shaotianlu@uniontech.com>
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

#pragma once

#include "interface/namespace.h"
#include "widgets/settingsitem.h"
#include "widgets/titlevalueitem.h"
#include "operation/systeminfomodel.h"

#include <DLineEdit>

#include <QFrame>

DCC_USE_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DToolButton;
class DLineEdit;
class DLabel;
DWIDGET_END_NAMESPACE

QT_BEGIN_NAMESPACE
class QLabel;
class QVBoxLayout;
class QHBoxLayout;

QT_END_NAMESPACE

namespace DCC_NAMESPACE {
class SystemInfoModel;
class TitleValueItem;

class HostNameEdit : public DTK_WIDGET_NAMESPACE::DLineEdit
{
Q_OBJECT
public:
    explicit HostNameEdit(QWidget *parent = nullptr);

protected:
    bool eventFilter(QObject *, QEvent *) override;
};

class HostNameItem : public SettingsItem
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
    Dtk::Widget::DToolButton *m_hostNameBtn;
    HostNameEdit *m_hostNameLineEdit;
    QString m_alertMessage;
    QString m_hostname;//保存计算机的全名
    QString m_hostnameEdit;//保存编辑时的数据
};

class NativeInfoWidget : public QFrame
{
    Q_OBJECT
public:
    explicit NativeInfoWidget(SystemInfoModel *model, QWidget *parent = nullptr);
    virtual ~NativeInfoWidget() override;

private:
    void initWidget();
    const QString systemCopyright() const;
    const QString typeStr(const QString &type) const;

    void setLicenseState(TitleAuthorizedItem *const authorized, DCC_NAMESPACE::ActiveState state);

Q_SIGNALS:
    void clickedActivator();

private:
    SystemInfoModel *m_model;
};

}
