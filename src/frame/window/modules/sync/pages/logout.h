// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "loginedin.h"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

namespace dcc {
namespace cloudsync {
class SyncModel;
}
}

namespace DCC_NAMESPACE {
namespace sync {
class LogoutPage : public LoginedIn
{
    Q_OBJECT
public:
    explicit LogoutPage(QWidget *parent = nullptr);
    void setModel(dcc::cloudsync::SyncModel *model);

Q_SIGNALS:
    void requestLogout() const;

private:
    QVBoxLayout *m_mainLayout;
};
}
}
