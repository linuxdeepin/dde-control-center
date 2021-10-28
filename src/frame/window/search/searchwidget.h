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
#include <com_deepin_wm.h>

#include <QWidget>
#include <QList>
#include <QLocale>
#include <QListView>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QGSettings>
#include <memory>

QT_BEGIN_NAMESPACE
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QXmlStreamReader;
QT_END_NAMESPACE

#define DEBUG_XML_SWITCH 0

const QString XML_Source = "source";
const QString XML_Title = "translation";
const QString XML_Numerusform = "numerusform";
const QString XML_Explain_Path = "extra-contents_path";
const QString XML_Child_Path = "extra-child_page";

using WM = com::deepin::wm;

namespace DCC_NAMESPACE {
namespace search {
struct SearchBoxStruct {
    typedef std::shared_ptr<SearchBoxStruct> Ptr;
    QString source;
    QString translateContent;
    QString actualModuleName;
    QString childPageName;
    QString fullPagePath;
};

struct SearchDataStruct {
    QString chiese;
    QString pinyin;
};

struct UnexsitStruct {
    QString module;
    QString datail;
};

class DCompleterStyledItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DCompleterStyledItemDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

class SearchModel : public QStandardItemModel {
    Q_OBJECT
    friend class SearchWidget;
public:
    explicit SearchModel(QObject* parent = nullptr);

private:
    QString formatSearchData(QString data1, QString data2, QString data3 = "");

public:
    int getDataNum(QString source, char value);
    bool jumpContentPathWidget(const QString &path);
    void setLanguage(const QString &type);
    void addModulesName(QString moduleName, const QString &searchName, QIcon icon, QString translation = "");
    void addUnExsitData(const QString &module = "", const QString &datail = "");
    void removeUnExsitData(const QString &module = "", const QString &datail = "");
    void setRemoveableDeviceStatus(const QString &name, bool isExist);
    void addSpecialThreeMenuMap(const QString &name, bool flag);

Q_SIGNALS:
    void notifyModuleSearch(QString, QString);

private:
    void loadxml();
    QString getModulesName(const QString &name, bool state = true);
    QString removeDigital(QString input);
    QString transPinyinToChinese(const QString &pinyin);
    QString containTxtData(QString txt);
    void appendChineseData(SearchBoxStruct::Ptr data);
    bool isLoadText(const QString &txt);
    bool isLoadContensText(const QString &text);

private:
    QList<SearchBoxStruct::Ptr> m_originList;
    QList<SearchBoxStruct::Ptr> m_EnterNewPagelist;
    QList<SearchBoxStruct::Ptr> m_hideList;
    QSet<QString> m_xmlFilePath;
    QString m_lang;
    QMap<QString, QIcon> m_iconMap;
    QList<QPair<QString, QString>> m_moduleNameList;//用于存储如 "update"和"Update"
    QList<SearchDataStruct> m_inputList;
    QList<UnexsitStruct>    m_unexsitList;
    QList<QPair<QString, bool>> m_serverTxtList;//QString表示和服务器/桌面版有关的文言,bool:true表示只有服务器版会存在,false表示只有桌面版存在
    QList<QString> m_childWidgetList; //二级页面list
    QList<QString> m_TxtListAll; //三级页面list
    QStringList m_defaultRemoveableList;//存储已知全部模块是否存在
    QList<QPair<QString, QString>> m_removedefaultWidgetList;//用于存储可以出设备名称，和该名称对应的页面
    QList<QPair<QString, QString>> m_removeableActualExistList;//存储实际模块是否存在
    QList<QPair<QString, bool>> m_contensServerTxtList;
    QMap<QString, bool> m_specialThreeMenuMap; //特别的三菜单显示
    bool m_bIsChinese;
    bool m_bIstextEdited;
    bool m_bIsServerType;
    bool m_bIsContensServerType;
    bool m_bIsOnBattery;
    bool m_bIsUseTouchpad;
    QGSettings *m_searchModuleDevelop{nullptr};
    WM *m_deepinwm;
    bool m_compositingAllowSwitch = true;
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
    void setRemoveableDeviceStatus(const QString &name, bool isExist);
    void addSpecialThreeMenuMap(const QString &name, bool flag);
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
