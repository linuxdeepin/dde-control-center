/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <wangwei_cm@deepin.com>
 *
 * Maintainer: andywang <wangwei_cm@deepin.com>
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
#include "window/utils.h"

#include <DListView>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QListView;
class QVBoxLayout;
class QStandardItemModel;
QT_END_NAMESPACE

namespace dcc {
namespace keyboard {
class KeyboardModel;
}

namespace widgets {
class MultiSelectListView;
}

namespace widgets {
class SettingsGroup;
class SwitchWidget;
class TitledSliderItem;
}
}

namespace DCC_NAMESPACE {
namespace keyboard {
class KeyboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KeyboardWidget(QWidget *parent = nullptr);
    ~KeyboardWidget();
    void initSetting(const int settingIndex);
    void setDefaultWidget();
private:
    void init();
    void onItemClick(const QModelIndex &index);
Q_SIGNALS:
    void showGeneralSetting();
    void showKBLayoutSetting();
    void showSystemLanguageSetting();
    void showShortCutSetting();
    void requestUpdateSecondMenu(const bool needPop);

private:
    QList<ListSubItem> m_itemList;
    dcc::keyboard::KeyboardModel *m_keyboardModel;
    dcc::widgets::MultiSelectListView *m_keyboardListView;
    QVBoxLayout *m_contentLayout;
    QStandardItemModel *m_listviewModel;
    QModelIndex m_lastIndex;
};
}
}
