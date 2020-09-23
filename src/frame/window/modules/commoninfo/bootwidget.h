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

#include <QVBoxLayout>
#include <QResizeEvent>

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

    void setDefaultEntry(const QString &value);
    void setModel(CommonInfoModel *model);
    void setBootList();

Q_SIGNALS:
    void enableTheme(bool value);
    void bootdelay(bool value);
    void defaultEntry(const QString &item);
    void requestSetBackground(const QString &path);

public Q_SLOTS:
    void setEntryList(const QStringList &list);
    void onCurrentItem(const QModelIndex &curIndex);

private:
    QString m_defaultEntry;                      // 默认启动项
    dcc::widgets::SwitchWidget *m_bootDelay;       // 延时启动功能
#ifndef DCC_DISABLE_GRUB_THEME
    dcc::widgets::SwitchWidget *m_theme;           // 主题功能
#endif
    DTK_WIDGET_NAMESPACE::DListView *m_bootList; // 启动项目列表
    dcc::widgets::TipsLabel *m_updatingLabel;    // Updating标签
    CommonBackgroundItem *m_background;          // 背景项
    QVBoxLayout *m_listLayout;
    QModelIndex m_curSelectedIndex;
    QStandardItemModel *m_bootItemModel;
    CommonInfoModel *m_commonInfoModel;
};

} // namespace commoninfo
} // namespace dcc
