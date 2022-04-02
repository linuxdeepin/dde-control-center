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
#include "widgets/titlelabel.h"

#include <DCommandLinkButton>
#include <DListView>
#include <DFloatingButton>
#include <DAnchors>

#include <QVBoxLayout>
#include <QPushButton>

DWIDGET_USE_NAMESPACE
namespace DCC_NAMESPACE {
class KeyboardModel;
class MetaData;
class SearchInput;
class ComboxWidget;

class KBLayoutListView : public DListView
{
    Q_OBJECT
public:
    explicit KBLayoutListView(QWidget *parent = nullptr): DListView(parent) {}

Q_SIGNALS:
    void currentChangedSignal(const QModelIndex &current);

protected:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous)
    {
        DListView::currentChanged(current, previous);
        Q_EMIT currentChangedSignal(current);
    }

    void mousePressEvent(QMouseEvent *event)
    {
        if(indexAt(event->pos()).row() == count() - 1)
            return;

        DListView::mousePressEvent(event);
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
    enum {
        SwitchValueRole = Dtk::UserRole + 1,
        KBLangIdRole
    };
};
}
#endif // KBLAYOUTSETTINGWIDGET_H
