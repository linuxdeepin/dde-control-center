//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"

#include <dtkwidget_global.h>

#include <QWidget>
#include <QVariant>

class QVBoxLayout;
class QLabel;

DWIDGET_BEGIN_NAMESPACE
class DIconButton;
DWIDGET_END_NAMESPACE

class ThemeItemPic;

class ThemeItem : public QWidget
{
    Q_OBJECT
public:
    explicit ThemeItem(bool titleBelowPic, QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setSelected(bool selected);
    void setPic(const QString &picPath);
    void setId(const QVariant &id);
    inline const QVariant id() const { return m_id; }

Q_SIGNALS:
    void selectedChanged(const bool selected) const;

private:
    QVBoxLayout *m_mainLayout;
    QLabel *m_title;
    bool m_state;
    ThemeItemPic *m_itemPic;  //picture of theme
    QVariant m_id;
    bool m_titleBelowPic;
    DTK_WIDGET_NAMESPACE::DIconButton *m_imgBtn;
};
