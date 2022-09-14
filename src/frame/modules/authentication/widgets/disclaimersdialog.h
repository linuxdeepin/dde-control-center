// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once
#include "interface/namespace.h"
#include "widgets/settingsitem.h"

#include <DAbstractDialog>
#include <DSuggestButton>
#include <DTipLabel>

DWIDGET_USE_NAMESPACE

QT_BEGIN_NAMESPACE
class QDialog;
class QVBoxLayout;
QT_END_NAMESPACE

namespace dcc {
namespace authentication {

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

Q_SIGNALS:
    /**
     * @brief requestClickStatus 点击确定后 返回登陆界面 显示勾选状态
     */
    void requestClickStatus(bool isClick);

private:
    QVBoxLayout *m_mainLayout;
    QPushButton* m_cancelBtn;
    DTK_WIDGET_NAMESPACE::DSuggestButton* m_acceptBtn;
};

}
}
