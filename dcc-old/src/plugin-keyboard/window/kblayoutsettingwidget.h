// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef KBLAYOUTSETTINGWIDGET_H
#define KBLAYOUTSETTINGWIDGET_H

#include "dcclistview.h"
#include "interface/namespace.h"
#include "widgets/titlelabel.h"

#include <DAnchors>
#include <DCommandLinkButton>
#include <DFloatingButton>
#include <DListView>

#include <QPushButton>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {
class KeyboardModel;
class SearchInput;
class ComboxWidget;

class KBLayoutListView : public DCCListView
{
    Q_OBJECT
public:
    explicit KBLayoutListView(QWidget *parent = nullptr)
        : DCCListView(parent)
    {
        setSpacing(0);
    }

Q_SIGNALS:
    void currentChangedSignal(const QModelIndex &current);

protected:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override
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
    void setModel(KeyboardModel *model);
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
    void onUpdateKBLayoutList();

private:
    bool m_bEdit = false;
    QStringList m_kbLangList;
    KeyboardModel *m_model;
    KBLayoutListView *m_kbLayoutListView;
    DCommandLinkButton *m_editKBLayout;
    QStandardItemModel *m_kbLayoutModel;
    DViewItemAction *m_addLayoutAction;

private:
    enum { SwitchValueRole = Dtk::UserRole + 1, KBLangIdRole };
};
} // namespace DCC_NAMESPACE
#endif // KBLAYOUTSETTINGWIDGET_H
