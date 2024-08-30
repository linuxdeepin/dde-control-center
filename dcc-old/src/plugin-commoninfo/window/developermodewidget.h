//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "interface/namespace.h"
#include "developermodedialog.h"

#include <QWidget>
#include <DLabel>

class QPushButton;
class QDBusInterface;

namespace DCC_NAMESPACE {
class SwitchWidget;
class CommonInfoModel;

class DeveloperModeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DeveloperModeWidget(QWidget *parent = nullptr);
    ~DeveloperModeWidget();
    void setModel(CommonInfoModel *model);

Q_SIGNALS:
    void enableDeveloperMode(bool enabled);
    void requestLogin();

private Q_SLOTS:
    void onLoginChanged();

public Q_SLOTS:
    void updateDeveloperModeState(const bool state);

private:
    bool m_enterDev{false};
    QPushButton *m_devBtn;
    CommonInfoModel *m_model{nullptr};    
    QDBusInterface *m_inter;
    DTK_NAMESPACE::Widget::DLabel *m_lab;
    DTK_NAMESPACE::Widget::DLabel *m_dtip;
    DeveloperModeDialog *m_developerDialog;
};
}
