/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     wuchuanfei <wuchuanfei_cm@deepin.com>
 *
 * Maintainer: wuchuanfei <wuchuanfei_cm@deepin.com>
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

#include <DListView>

#include <QResizeEvent>

class QVBoxLayout;
class QLabel;
class QScrollArea;

DWIDGET_BEGIN_NAMESPACE
class DTipLabel;
class DDialog;
class DCommandLinkButton;
DWIDGET_END_NAMESPACE

namespace DCC_NAMESPACE {
class SwitchWidget;

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
    SwitchWidget *m_bootDelay;       // 延时启动功能
#ifndef DCC_DISABLE_GRUB_THEME
    SwitchWidget *m_theme;           // 主题功能
    DTK_WIDGET_NAMESPACE::DTipLabel *m_themeLbl;    // 主题提示
#endif
    SwitchWidget *m_grubVerification;    // grub 验证
    DTK_WIDGET_NAMESPACE::DTipLabel *m_grubVerifyLbl;  // grub 验证提示
    DTK_WIDGET_NAMESPACE::DCommandLinkButton *m_grubModifyPasswdLink; // grub修改密码
    DTK_WIDGET_NAMESPACE::DDialog *m_grubEditAuthDialog = nullptr; // grub修改密码输入框
    DTK_WIDGET_NAMESPACE::DListView *m_bootList; // 启动项目列表
    QLabel *m_updatingLabel;    // Updating标签
    CommonBackgroundItem *m_background;          // 背景项
    QVBoxLayout *m_listLayout;
    QModelIndex m_curSelectedIndex;
    QStandardItemModel *m_bootItemModel;
    CommonInfoModel *m_commonInfoModel = nullptr;
    bool m_isCommoninfoBootWallpaperConfigValid;
    QScrollArea *m_scrollArea; // 添加滚动区域
};
} // namespace DCC_NAMESPACE

Q_DECLARE_METATYPE(QMargins)
