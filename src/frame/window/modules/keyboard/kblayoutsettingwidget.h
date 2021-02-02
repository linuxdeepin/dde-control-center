/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <andywang_cm@deepin.com>
 *
 * Maintainer: andywang <andywang_cm@deepin.com>
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
#ifndef KBLAYOUTSETTINGWIDGET_H
#define KBLAYOUTSETTINGWIDGET_H

#include "interface/namespace.h"
#include "widgets/contentwidget.h"

#include <DCommandLinkButton>
#include <DListView>

QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE

namespace dcc {
namespace keyboard {
class KeyboardModel;
class MetaData;
}

namespace widgets {
class SearchInput;
class TranslucentFrame;
class ComboxWidget;
}
}

namespace DCC_NAMESPACE {
namespace keyboard {
class CheckItem;

class MyListView : public DListView
{
    Q_OBJECT
public:
    explicit MyListView(QWidget *parent = nullptr): DListView(parent) {}

Q_SIGNALS:
    void currentChangedSignal(const QModelIndex &current);

    // QAbstractItemView interface
protected:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous)
    {
        DListView::currentChanged(current, previous);
        Q_EMIT currentChangedSignal(current);
    }
};

class KBLayoutSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KBLayoutSettingWidget(QWidget *parent = nullptr);
    void setModel(dcc::keyboard::KeyboardModel *model);

Q_SIGNALS:
    void layoutAdded(const QStringList &kblist);
    void requestCurLayoutAdded(const QString &value);
    void curLang(const QString &value);
    void delUserLayout(const QString &value);
    void requestSwitchKBLayout(int value);
    void onSwitchKBLayoutScope(const int index);

public Q_SLOTS:
    void onAddKeyboard(const QString &id, const QString &value);
    void onEditClicked();
    void onDefault(const QString &value);
    void onKBLayoutChanged(const QModelIndex &index);
    void onKBCurrentChanged(const QModelIndex &current);
    void onSwitchKBChanged(const QModelIndex &index);
    void onSwitchKB(int kbSwitch);
    void onLayoutAdded();
    void onLayoutScope(const int value);
private:
    void setUIVisible();
private:
    bool m_bEdit = false;
    QList<dcc::keyboard::MetaData> m_datas;
    QStringList m_kbLangList;
    dcc::keyboard::KeyboardModel *m_model;
    dcc::widgets::ComboxWidget *m_comboWidget;
    QLabel *m_switchTitle;
    MyListView *m_kbLayoutListView;
    DListView *m_switchLayoutListView;
    DCommandLinkButton *m_editKBLayout;
    QStandardItemModel *m_kbLayoutModel;
    QStandardItemModel *m_switchLayoutModel;
    dcc::ContentWidget *m_contentWidget;
private:
    enum {
        SwitchValueRole = Dtk::UserRole + 1,
        KBLangIdRole
    };
};
}
}
#endif // KBLAYOUTSETTINGWIDGET_H
