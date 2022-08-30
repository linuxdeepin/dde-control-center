// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
    int showPath(const QString &path);
private:
    void init();
    void onItemClick(const QModelIndex &index);
    int getListIndex(QString data);
    bool configContent(const QString & configName);
Q_SIGNALS:
    void showGeneralSetting();
    void showKBLayoutSetting();
    void showSystemLanguageSetting();
    void showShortCutSetting();
    void requestUpdateSecondMenu(const bool needPop);
    void notifyEnterSearchWidget(const QString &search);

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
