//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
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

#include <dtkwidget_global.h>
#include "interface/namespace.h"
#include "widgets/settingsitem.h"
#include <DPalette>

QT_BEGIN_NAMESPACE
class QWidget;
class QHBoxLayout;
class QStackedWidget;
class QLabel;
class QLineEdit;
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DIconButton;
class DLineEdit;
DWIDGET_END_NAMESPACE

class AuthenticationLinkButtonItem : public DCC_NAMESPACE::SettingsItem{
    Q_OBJECT
public:
    explicit AuthenticationLinkButtonItem(QWidget *parent = nullptr);

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

Q_SIGNALS:
    void mousePressed();

private:
    DTK_GUI_NAMESPACE::DPalette m_currentpa;
};

class AuthenticationInfoItem : public DCC_NAMESPACE::SettingsItem
{
    Q_OBJECT
public:
    explicit AuthenticationInfoItem(QWidget *parent = nullptr);
    void setTitle(const QString &title);
    QString getTitle() const { return m_itemName; };
    void alertTitleRepeat();
    void appendItem(QWidget *widget);
    void setShowIcon(bool state);
    void setEditTitle(bool state);
    void setHideTitle(bool state);
    bool onNameEditFinished();
    //判断账户名是否符合规则
    bool validateName(const QString &password);
    void showAlertMessage(const QString &errMsg);

Q_SIGNALS:
    void removeClicked();
    void editClicked(bool state);
    void editTextFinished(QString finger);

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QHBoxLayout *m_layout;
    QLabel *m_title;
    DTK_WIDGET_NAMESPACE::DIconButton *m_removeBtn;
    DTK_WIDGET_NAMESPACE::DIconButton *m_editBtn;
    DTK_WIDGET_NAMESPACE::DLineEdit *m_editTitle;
    QString m_itemName;
    DTK_GUI_NAMESPACE::DPalette m_currentpa;
};
