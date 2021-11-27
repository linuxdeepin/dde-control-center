/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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
    void addUnExsitData(const QString &module = "", const QString &datail = "");
    void removeUnExsitData(const QString &module = "", const QString &datail = "");
    void addSpecialThreeMenuMap(const QString &name, bool flag);
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
