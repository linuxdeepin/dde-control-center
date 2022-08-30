// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
    void creatDelIconAction(DStandardItem *item);

Q_SIGNALS:
    void layoutAdded(const QStringList &kblist);
    void requestCurLayoutAdded(const QString &value);
    void curLang(const QString &value);
    void delUserLayout(const QString &value);

public Q_SLOTS:
    void onAddKeyboard(const QString &id, const QString &value);
    void onEditClicked();
    void onDefault(const QString &value);
    void onKBLayoutChanged(const QModelIndex &index);
    void onKBCurrentChanged(const QModelIndex &current);
    void onLayoutAdded();

private:
    bool m_bEdit = false;
    QStringList m_kbLangList;
    dcc::keyboard::KeyboardModel *m_model;
    MyListView *m_kbLayoutListView;
    DCommandLinkButton *m_editKBLayout;
    QStandardItemModel *m_kbLayoutModel;
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
