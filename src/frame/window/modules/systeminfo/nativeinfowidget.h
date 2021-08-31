/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
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
#include "widgets/titlevalueitem.h"
#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/settingsitem.h"
#include "modules/systeminfo/systeminfomodel.h"

#include <DLineEdit>

#include <QWidget>
#include <QGSettings>

using namespace dcc;

DWIDGET_BEGIN_NAMESPACE
class DToolButton;
class DLineEdit;
class DLabel;
DWIDGET_END_NAMESPACE

QT_BEGIN_NAMESPACE

class QVBoxLayout;
class QHBoxLayout;

QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace systeminfo {

class HostNameEdit : public DTK_WIDGET_NAMESPACE::DLineEdit
{
Q_OBJECT
public:
    explicit HostNameEdit(QWidget *parent = 0);

protected:
    bool eventFilter(QObject *, QEvent *) override;
};
class NativeInfoWidget : public ContentWidget
{
    Q_OBJECT
public:
    explicit NativeInfoWidget(dcc::systeminfo::SystemInfoModel *model, QWidget *parent = nullptr);
    virtual ~NativeInfoWidget();

private:
    void initWidget();
    const QString systemCopyright() const;
    const QString systemLogo() const;
    const QString getElidedText(QWidget* widget, QString data, Qt::TextElideMode mode = Qt::ElideRight, int width = 100, int flags = 0, int line = 0);

public Q_SLOTS:
    void setProductName(const QString &edition);
    void setVersionNumber(const QString &type);
    void setEdition(const QString &edition);
    void setType(const QString &type);
    void setProcessor(const QString &processor);
    void setMemory(const QString &memory);
    void setLicenseState(dcc::systeminfo::ActiveState state);

    void onToolButtonButtonClicked();
    void onFocusChanged(const bool onFocus);
    void onTextEdited(const QString &hostName);
    void onAlertChanged();
    void onEditingFinished();
    void onHostNameChanged(const QString &hostName);
    void onSetHostNameError(const QString &error);

Q_SIGNALS:
    void clickedActivator();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    dcc::systeminfo::SystemInfoModel *m_model;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_hostNameLayout;
    dcc::widgets::SettingsItem *m_hostNameSettingItem;
    DLabel *m_hostNameTitleLabel;//计算机名标题
    DLabel *m_hostNameLabel;//计算机名显示
    DTK_WIDGET_NAMESPACE::DToolButton *m_hostNameBtn;//计算机名编辑按钮
    HostNameEdit *m_hostNameLineEdit;//计算机名编辑框
    dcc::widgets::TitleValueItem *m_productName;
    dcc::widgets::TitleValueItem *m_versionNumber;
    dcc::widgets::TitleValueItem *m_version;
    dcc::widgets::TitleValueItem *m_type;
    dcc::widgets::TitleAuthorizedItem *m_authorized;
    dcc::widgets::TitleValueItem *m_kernel;
    dcc::widgets::TitleValueItem *m_processor;
    dcc::widgets::TitleValueItem *m_memory;
    QGSettings *m_moduleActive{nullptr};
    bool isContensServers;
    QString m_alertMessage;
    QString m_hostname;//保存计算机的全名
    QString m_hostnameEdit;//保存编辑时的数据
};

}
}
