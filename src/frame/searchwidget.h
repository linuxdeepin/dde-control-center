/*
* Copyright (C) 2019 ~ 2022 Deepin Technology Co., Ltd.
*
* Author:     guoyao <guoyao@uniontech.com>

* Maintainer: guoyao <guoyao@uniontech.com>

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include "interface/namespace.h"

#include <DSearchEdit>
#include <QCompleter>
#include <QSet>
#include <QStyledItemDelegate>

class QStandardItemModel;

namespace DCC_NAMESPACE {

class ModuleObject;

class DccCompleter : public QCompleter
{
public:
    DccCompleter(QObject *parent = nullptr);
    DccCompleter(QAbstractItemModel *model, QObject *parent = nullptr);

public:
    bool eventFilter(QObject *o, QEvent *e) override;
};

class DccCompleterStyledItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DccCompleterStyledItemDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

class SearchWidget : public Dtk::Widget::DSearchEdit
{
    Q_OBJECT
public:
    explicit SearchWidget(QWidget *parent = nullptr);

    void setModuleObject(ModuleObject *const module);
    QList<QPair<QString, QString>> searchResults(const QString text);

    void addModule(ModuleObject *const module);
    void removeModule(ModuleObject *const module);

signals:
    void notifySearchUrl(const QString &url);

private:
    QString convertUrl(const QStringList &displayNames);

private slots:
    void onReturnPressed();
    void onSearchTextChange(const QString &text);
    void onAutoComplete(const QString &text);

private:
    QStandardItemModel *m_model;
    DccCompleter *m_completer;
    ModuleObject *m_rootModule;
    bool m_bIsChinese;
    QSet<QString> m_allText; // 去重
};

} // namespace DCC_NAMESPACE
