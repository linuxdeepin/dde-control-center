/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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

#include <dtkwidget_global.h>

#include <DAbstractDialog>
#include <DStyleOption>

#include <QDialog>
#include <QJsonObject>

class ThemeModel;

DWIDGET_BEGIN_NAMESPACE
class DListView;
DWIDGET_END_NAMESPACE

class PersonalizationThemeList : public DTK_WIDGET_NAMESPACE::DAbstractDialog
{
    Q_OBJECT
public:
    explicit PersonalizationThemeList(const QString &title, QWidget *parent = nullptr);
    ~PersonalizationThemeList();

    void setModel(ThemeModel *const model);

Q_SIGNALS:
    void requestSetDefault(const QJsonObject &value);

public Q_SLOTS:
    void setDefault(const QString &name);
    void onSetPic(const QString &id, const QString &picPath);
    void onAddItem(const QJsonObject &json);
    void onRemoveItem(const QString &id);
    void onClicked(const QModelIndex &);
    void clickSaveBtn();

private:
    enum PersonalizationItemDataRole {
        IDRole = DTK_NAMESPACE::UserRole + 1,
    };

protected:
    QMap<QString, QJsonObject> m_jsonMap;
    ThemeModel *m_model;
    DTK_WIDGET_NAMESPACE::DListView *m_listview;
};
