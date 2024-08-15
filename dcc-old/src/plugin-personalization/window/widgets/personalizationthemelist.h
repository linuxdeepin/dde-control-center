//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"

#include <dtkwidget_global.h>
#include <DStyleOption>
#include <DAbstractDialog>

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
