// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"

#include <dtkwidget_global.h>
#include <DStyleOption>

#include <QWidget>
#include <QJsonObject>

namespace dcc {
namespace personalization {
class ThemeModel;
}
}

DWIDGET_BEGIN_NAMESPACE
class DListView;
DWIDGET_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace personalization {
class PerssonalizationThemeList : public QWidget
{
    Q_OBJECT
public:
    explicit PerssonalizationThemeList(QWidget *parent = nullptr);
    ~PerssonalizationThemeList();

    void setModel(dcc::personalization::ThemeModel *const model);

Q_SIGNALS:
    void requestSetDefault(const QJsonObject &value);

public Q_SLOTS:
    void setDefault(const QString &name);
    void onSetPic(const QString &id, const QString &picPath);
    void onAddItem(const QJsonObject &json);
    void onRemoveItem(const QString &id);
    void onClicked(const QModelIndex &);

private:
    enum PersonalizationItemDataRole{
        IDRole = DTK_NAMESPACE::UserRole + 1,
    };

protected:
    QMap<QString, QJsonObject> m_jsonMap;
    dcc::personalization::ThemeModel *m_model;
    DTK_WIDGET_NAMESPACE::DListView *m_listview;
};
}
}
