// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef COMPLETERVIEW_H
#define COMPLETERVIEW_H
#include "interface/namespace.h"

#include <DGuiApplicationHelper>
#include <DListView>

namespace DCC_NAMESPACE {
class CompleterView : public Dtk::Widget::DListView
{
public:
    CompleterView(QWidget *parent = nullptr);
    void updateViewportMargins();
    int margin() const;

private:
    Dtk::Gui::DGuiApplicationHelper::SizeMode m_sizeMode;
};
} // namespace DCC_NAMESPACE

#endif // COMPLETERVIEW_H
