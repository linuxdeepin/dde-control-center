// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"

#include "dsearchedit.h"

#include <QWidget>
#include <QLocale>
#include <QListView>
#include <QStyledItemDelegate>
QT_BEGIN_NAMESPACE
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QXmlStreamReader;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace search {
class SearchModel;

class DCompleterStyledItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DCompleterStyledItemDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
class SearchWidget : public DTK_WIDGET_NAMESPACE::DSearchEdit
{
    Q_OBJECT
public:
    SearchWidget(QWidget *parent = nullptr);
    ~SearchWidget() override;

    bool jumpContentPathWidget(const QString &path);
    void setLanguage(const QString &type);
    void addModulesName(QString moduleName, const QString &searchName, QIcon icon, QString translation = "");

    QList<QString> searchResults(const QString text);
    void getJumpPath(QString &moduleName, QString &pageName, const QString &searchName);
    void setModuleVisible(const QString &module, bool visible);
    void setWidgetVisible(const QString &module, const QString &widget, bool visible);
    void setDetailVisible(const QString &module, const QString &widget, const QString &detail, bool visible);
    void updateSearchdata(const QString &module);
    void addChildPageTrans(const QString &menu, const QString &tran);

private Q_SLOTS:
    void onCompleterActivated(const QString &value);
    void onAutoComplete(const QString &text);
    void onSearchTextChange(const QString &text);
Q_SIGNALS:
    void notifyModuleSearch(QString, QString);

private:
    SearchModel *m_model;
    QCompleter *m_completer;
    DCompleterStyledItemDelegate styledItemDelegate;
    QStringList m_forbidTextList;
};

}// namespace search
}// namespace DCC_NAMESPACE
