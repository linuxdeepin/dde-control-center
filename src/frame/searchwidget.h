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
#include <QStyledItemDelegate>

class QStandardItemModel;

namespace DCC_NAMESPACE
{

class ModuleObject;

/**
 * @brief SearchData 为搜索数据的基本数据储存方式,每个SearchData对应一条搜索数据
 */
class SearchData : public QObject
{
    Q_OBJECT
public:
    QString Url;        // 不可见,将该数据发送给MainWindow即可跳转到相应的界面
    QString SearchUrl;  // 最终显示结果
    QString PinYin;     // 不可见,在中文环境下,对拼音进行的处理,其格式与SearchUrl一致,但内容为拼音
    QList<ModuleObject *> ModuleUrl; // 不可见，Url对应的模块地址
    SearchData(QObject *parent = nullptr);
    SearchData(const QString &url, const QString &searchUrl = QString() , const QString &pinYin = QString(), const QList<ModuleObject *> &moduleUrl = {}, QObject *parent = nullptr);
    SearchData(const SearchData &m);
    inline SearchData &operator=(const SearchData &other);
    inline bool operator==(const SearchData &other) const;
    inline QDebug operator<<(QDebug d) const;
};

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
    SearchWidget(QWidget *parent = nullptr);

    void setModuleObject(ModuleObject *const module);

signals:
    void notifySearchUrl(const QString &url);

private:
    void refreshModel();

    /**
     * @brief 将模块及其所有子项拼接为 Url, 并保存在 m_searchData 中
     * @param module 模块对象
     * @param prefix 前缀
     */
    void addUrl(ModuleObject * module, const QString &prefix = {}, QList<ModuleObject *> moduleUrl = {});

    /**
     * @brief 替换搜索数据, 删除原有数据, 并将新数据添加到 m_searchData 中
     * @param module 数据节点, url中某个字段
     * @param sectionIndex 节点索引, 避免误删同名节点
     */
    void replaceUrl(ModuleObject *const module);

    /**
     * @brief 获取当前模块的前缀
     * @param module 当前模块对象
     * @param child 从某个模块开始遍历
     * @param prefix 前缀
     */
    QString getPrefix(ModuleObject *const module, ModuleObject *const child, const QString &prefix = {});
    void configConnect(ModuleObject *const module);
    QString convertUrl(const QString& url);
    QString convertPinyin(const QString& url);
    QString removeDigital(const QString &word);

private slots:
    void onReturnPressed();
    void onSearchTextChange(const QString &text);
    void onAutoComplete(const QString &text);

private:
    QStandardItemModel      *m_model;
    DccCompleter            *m_completer;
    ModuleObject            *m_rootModule;
    QList<SearchData>       m_searchData;
    int                     m_insertIndex;
    bool                    m_bIsChinese;
};

} // namespace DCC_NAMESPACE
