// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once
#include "interface/namespace.h"

#include <DListView>

#include <QVBoxLayout>
#include <QResizeEvent>
#include <QScrollArea>

#include <QGSettings>

DWIDGET_BEGIN_NAMESPACE
class DTipLabel;
class DDialog;
class DCommandLinkButton;
DWIDGET_END_NAMESPACE

namespace dcc {
namespace widgets {
class SwitchWidget;
class TipsLabel;
}
}

namespace DCC_NAMESPACE {

namespace commoninfo {
class CommonInfoModel;
class CommonBackgroundItem;

class BootWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BootWidget(QWidget *parent = nullptr);
    virtual ~BootWidget();

    void setDefaultEntry(const QString &value);
    void setModel(CommonInfoModel *model);
    void resizeEvent(QResizeEvent *event);
    void setBootList();

Q_SIGNALS:
    void enableTheme(bool value);
    void bootdelay(bool value);
    void defaultEntry(const QString &item);
    void requestSetBackground(const QString &path);
    void enableGrubEditAuth(bool value);
    void setGrubEditPasswd(const QString &passwd, const bool &isReset);

public Q_SLOTS:
    void setEntryList(const QStringList &list);
    void onCurrentItem(const QModelIndex &curIndex);
    void onGrubEditAuthCancel(bool toEnable);
    void setGrubEditAuthVisible(bool show);
    void showGrubEditAuthPasswdDialog(bool isReset);

private:
    QString m_defaultEntry;                      // 默认启动项
    dcc::widgets::SwitchWidget *m_bootDelay;       // 延时启动功能
#ifndef DCC_DISABLE_GRUB_THEME
    dcc::widgets::SwitchWidget *m_theme;           // 主题功能
    DTK_WIDGET_NAMESPACE::DTipLabel *m_themeLbl;    // 主题提示
    QGSettings *m_commoninfoBootWallpaperConfigSetting{nullptr};
#endif
    dcc::widgets::SwitchWidget *m_grubVerification;    // grub 验证
    DTK_WIDGET_NAMESPACE::DTipLabel *m_grubVerifyLbl;  // grub 验证提示
    DTK_WIDGET_NAMESPACE::DCommandLinkButton *m_grubModifyPasswdLink; // grub修改密码
    DTK_WIDGET_NAMESPACE::DDialog *m_grubEditAuthDialog = nullptr; // grub修改密码输入框
    DTK_WIDGET_NAMESPACE::DListView *m_bootList; // 启动项目列表
    dcc::widgets::TipsLabel *m_updatingLabel;    // Updating标签
    CommonBackgroundItem *m_background;          // 背景项
    QVBoxLayout *m_listLayout;
    QModelIndex m_curSelectedIndex;
    QStandardItemModel *m_bootItemModel;
    CommonInfoModel *m_commonInfoModel = nullptr;
    bool m_isCommoninfoBootWallpaperConfigValid;
    QScrollArea *m_scrollArea; // 添加滚动区域
};

} // namespace commoninfo
} // namespace dcc
