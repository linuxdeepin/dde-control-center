//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"
#include <DLabel>

namespace DCC_NAMESPACE
{

class TitleLabel : public Dtk::Widget::DLabel
{
    Q_OBJECT
public:
    TitleLabel(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    TitleLabel(const QString &text, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

protected:
    bool event(QEvent *e) override;
};

} // namespace DCC_NAMESPACE
