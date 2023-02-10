//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "interface/namespace.h"

#include <QWidget>


namespace DCC_NAMESPACE {
class SwitchWidget;
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
    SwitchWidget *m_joinUeProgram;
};
}
