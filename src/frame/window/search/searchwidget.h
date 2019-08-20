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

#include "window/namespace.h"

#include "dsearchedit.h"

#include <QWidget>
#include <QList>
#include <QLocale>
#include <QListView>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QXmlStreamReader;
QT_END_NAMESPACE

#define DEBUG_XML_SWITCH 0

const QString RES_TS_PATH = ":/translations/";
const QString XML_Title = "translation";
const QString XML_Explain_Path = "extra-contents_path";

namespace DCC_NAMESPACE {
namespace search {

class SearchWidget : public DTK_WIDGET_NAMESPACE::DSearchEdit
{
    Q_OBJECT
public:
    struct SearchBoxStruct {
        QString title;
        QString explain;
    };

    struct EnterPageStruct {
        QString source;
        bool b_enter_new_page;
    };

public:
    SearchWidget(QWidget *parent = nullptr);
    ~SearchWidget() override;

    bool jumpContentPathWidget(QString path);
    void setLanguage(QString type);

private:
    void loadxml();
    SearchBoxStruct getModuleBtnString(QString value);
    QString getXmlFilePath();

private:
    QLineEdit *m_inputEdit;
    QStandardItemModel *m_model;
    QCompleter *m_completer;
    QList<SearchBoxStruct> m_EnterNewPagelist;
    SearchBoxStruct m_searchBoxStruct;
    QString m_xmlExplain;
    QString m_xmlFilePath;

Q_SIGNALS:
    void notifyModuleSearch(QString, QString);

private Q_SLOTS:
};

}// namespace search
}// namespace DCC_NAMESPACE
