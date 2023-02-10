//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "themeitem.h"

#include <QWidget>
#include <QBoxLayout>
#include <QJsonObject>

class ThemeModel;

class ThemeItem;
class PersonalizationThemeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PersonalizationThemeWidget(QWidget *parent = nullptr);
    void setModel(ThemeModel *const model);
    void setMainLayout(QBoxLayout *layout, bool titleBelowPic);

Q_SIGNALS:
    void requestSetDefault(const QJsonObject &value);

public Q_SLOTS:
    void setDefault(const QString &name);
    void onItemClicked(const bool selected);
    void onSetPic(const QString &id, const QString &picPath);
    void onAddItem(const QJsonObject &json);
    void onRemoveItem(const QString &id);
protected:
    void mouseMoveEvent(QMouseEvent* event)override;
    QBoxLayout *m_centerLayout;
    QMap<ThemeItem *, QJsonObject> m_valueMap;
    ThemeModel *m_model;
    bool m_titleBelowPic;
};
