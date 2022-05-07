/*
* Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#ifndef PERSONALIZATIONTHEMEMODULE_H
#define PERSONALIZATIONTHEMEMODULE_H
#include "interface/moduleobject.h"

DCC_BEGIN_NAMESPACE
class DCCListView;
DCC_END_NAMESPACE;
class QStandardItemModel;

class ThemeModel;
class PersonalizationWorker;

class PersonalizationThemeModule : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    explicit PersonalizationThemeModule(ThemeModel *model, PersonalizationWorker *work, QObject *parent = nullptr);
    void initThemeListView(DCC_NAMESPACE::DCCListView *listview);

public Q_SLOTS:
    void setDefault(const QString &name);
    void onSetPic(const QString &id, const QString &picPath);
    void onAddItem(const QJsonObject &json);
    void onRemoveItem(const QString &id);
    void onClicked(const QModelIndex &);

private:
    QStandardItemModel *m_itemModel;

    ThemeModel *m_model;
    PersonalizationWorker *m_work;

    QMap<QString, QJsonObject> m_jsonMap;
};

#endif // PERSONALIZATIONTHEMEMODULE_H
