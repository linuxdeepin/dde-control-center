// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"

#include <QWidget>
#include <QProcess>

class QVBoxLayout;

namespace DCC_NAMESPACE {
class MainWindow;
namespace sync {
class LoginPage : public QWidget
{
    Q_OBJECT
public:
    LoginPage(QWidget *parent = nullptr);
    void setMainWindow(MainWindow *pMainWindow);

private Q_SLOT:

Q_SIGNALS:
    void requestLoginUser() const;

private:
    QVBoxLayout *m_mainLayout;
    MainWindow *m_pMainWindow;
    QProcess *m_licenceProcess;
};
} // namespace sync
} // namespace DCC_NAMESPACE
