//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"

#include <DSearchEdit>
#include <QCompleter>
#include <QSet>
#include <QStyledItemDelegate>

class QStandardItemModel;

namespace DCC_NAMESPACE {

class ModuleObject;
class CompleterView;

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

protected:
    bool helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index) override;
    QRect calculateIconRect(const QRect &rect) const;
    QRect calculateTextRect(const QRect &rect) const;
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
    CompleterView *m_completerView;
    ModuleObject *m_rootModule;
    bool m_bIsChinese;
    QSet<QString> m_allText; // 去重
};

} // namespace DCC_NAMESPACE
