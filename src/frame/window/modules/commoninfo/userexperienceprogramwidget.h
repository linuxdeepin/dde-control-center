// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once
#include "interface/namespace.h"

#include <QWidget>

namespace dcc {
namespace widgets {
class SwitchWidget;
}
}

namespace DCC_NAMESPACE {
namespace commoninfo {
class CommonInfoModel;

class UserExperienceProgramWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UserExperienceProgramWidget(QWidget *parent = nullptr);

    void setModel(CommonInfoModel *model);
    void setDefaultUeProgram(const bool enabled);
Q_SIGNALS:
    void enableUeProgram(bool enabled);

private:
    dcc::widgets::SwitchWidget *m_joinUeProgram;
};
}
}
