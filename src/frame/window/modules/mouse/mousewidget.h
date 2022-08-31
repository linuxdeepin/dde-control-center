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

DWIDGET_BEGIN_NAMESPACE
class DStandardItem;
DWIDGET_END_NAMESPACE

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace mouse {
class MouseModel;
}
}

namespace dcc {
namespace widgets {
class SettingsGroup;
class SwitchWidget;
class TitledSliderItem;
class MultiSelectListView;
}
}

namespace DCC_NAMESPACE {
namespace mouse {
class MouseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MouseWidget(QWidget *parent = nullptr);
    ~MouseWidget();
    void init(bool tpadExist, bool redPointExist);
    void initSetting(const int settingIndex);
    void setDefaultWidget();
private:
    void onItemClicked(const QModelIndex &index);
    bool configContent(const QString & configName);
Q_SIGNALS:
    void tpadExistChanged(bool bExist);
    void redPointExistChanged(bool bExist);
    void showGeneralSetting();
    void showMouseSetting();
    void showTouchpadSetting();
    void showTrackPointSetting();
    void requestUpdateSecondMenu(const bool needPop);

private:
    dcc::mouse::MouseModel *m_mouseModel;
    dcc::widgets::MultiSelectListView *m_mouseListView;
    QVBoxLayout *m_contentLayout;
    QStandardItemModel *m_listviewModel;
    QModelIndex m_lastIndex;
    QList<ListSubItem> m_menuIconText;
    QList<DStandardItem *> m_mouseItems;
};
}
}
