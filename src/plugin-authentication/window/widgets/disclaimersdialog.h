//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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
#include "widgets/settingsitem.h"

#include <DAbstractDialog>
#include <QCloseEvent>

DWIDGET_USE_NAMESPACE

QT_BEGIN_NAMESPACE
class QDialog;
class QVBoxLayout;
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DSuggestButton;
class DTipLabel;
DWIDGET_END_NAMESPACE

enum DisclaimersObj {
    Faceid,
    Iris,
    Finge
};

// 免责声明对话框
class DisclaimersDialog : public DTK_WIDGET_NAMESPACE::DAbstractDialog
{
    Q_OBJECT
public:
    explicit DisclaimersDialog(DisclaimersObj disobj, DAbstractDialog *parent = nullptr);
    ~DisclaimersDialog();

private:
    void initWidget(DisclaimersObj state);
    void initConnect();

protected:
    void closeEvent(QCloseEvent *event) override;

Q_SIGNALS:
    /**
     * @brief requestClickStatus 点击确定后 返回登陆界面 显示勾选状态
     */
    void requestClickStatus(bool isClick);
    /**
     * @brief requesetCloseDlg 离开免责对话框界面 需要恢复父窗口显示状态
     */
    void requesetCloseDlg(bool isClose);

private:
    QVBoxLayout *m_mainLayout;
    QPushButton* m_cancelBtn;
    DTK_WIDGET_NAMESPACE::DSuggestButton* m_acceptBtn;
};
