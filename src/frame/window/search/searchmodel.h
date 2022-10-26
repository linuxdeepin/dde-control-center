// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"

#include <QStandardItemModel>
#include <QSet>
#include <QLabel>

#include <memory>

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

struct HideChildWidgetStruct {
    QString module;                                     //子页面的模块
    QMap<QString, bool> childWidgetMap;              //子页面名称, 子页面是否显示
};

struct HideChildWidgetDetailStruct {
    QString module;                                     //子页面的模块，避免重复子页面名称
    QString childWidget;                                //子页面名称
    QMap<QString, bool> detailMap;                     //详细搜索数据是否显示
};


class SearchModel : public QStandardItemModel {
    Q_OBJECT
    friend class SearchWidget;
public:
    explicit SearchModel(QObject* parent = nullptr);
    ~SearchModel();

public:
    int getDataNum(QString source, char value);
    bool jumpContentPathWidget(const QString &path);
    void setLanguage(const QString &type);
    void addModulesName(QString moduleName, const QString &searchName, QIcon icon, QString translation = "");
    bool getModuleVisible(const QString module);
    bool getWidgetVisible(const QString module, QString widget = "");
    bool getDetailVisible(const QString module, QString widget = "", QString detail = "");
    void setModuleVisible(const QString &module, bool visible);
    void setWidgetVisible(const QString &module, const QString &widget, bool visible);
    void setDetailVisible(const QString &module, const QString &widget, const QString &detail, bool visible);
    void updateSearchData(const QString &module);
    void getJumpPath(QString &moduleName, QString &pageName, const QString &searchName);
    inline bool getDataUpdateCompleted() { return m_dataUpdateCompleted; }
    void addChildPageTrans(const QString &menu, const QString &tran);
    QString getRealTxt(const QString &key) const;

Q_SIGNALS:
    void notifyModuleSearch(QString, QString);

private:
    void loadxml(const QString module = "");
    QString getModulesName(const QString &name, bool state = true);
    QString removeDigital(QString input);
    QString transPinyinToChinese(const QString &pinyin);
    QString containTxtData(QString txt);
    void appendChineseData(SearchBoxStruct::Ptr data);
    SearchBoxStruct::Ptr getModuleBtnString(QString value);
    bool specialProcessData(SearchBoxStruct::Ptr data);
    QString getNormalText(QString value);

private:
    QList<SearchBoxStruct::Ptr> m_originList;
    QList<SearchBoxStruct::Ptr> m_EnterNewPagelist;
    QList<SearchBoxStruct::Ptr> m_hideList;
    QSet<QString> m_xmlFilePath;
    QString m_lang;
    QMap<QString, QIcon> m_iconMap;
    QList<QPair<QString, QString>> m_moduleNameList;//用于存储如 "update"和"Update"
    QList<SearchDataStruct> m_inputList;
    QList<QString> m_childWidgetList; //二级页面list
    QList<QString> m_childeHideWidgetList; //不需要显示的二级页面list，比如 “默认程序 --> 终端 / 添加默认程序” 和 “默认程序 --> 终端”
    QList<QString> m_TxtListAll; //三级页面list
    QList<QPair<QString, QString>> m_removeableActualExistList;//存储实际模块是否存在
    bool m_bIsChinese;
    bool m_bIstextEdited;
    QMap<QString, bool> m_hideModuleList;
    QList<HideChildWidgetStruct> m_hideWidgetList;
    QList<HideChildWidgetDetailStruct> m_hideWidgetDetailList;
    QMap<QString, QString> m_transChildPageName;
    bool m_dataUpdateCompleted;
    QMap<QString, QString> m_transPlusData;
    QLabel *m_calLenthLabel;
    QMap<QString, QString> m_scaleTxtMap;
};

}// namespace search
}// namespace DCC_NAMESPACE
