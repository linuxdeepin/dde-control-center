//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "titlemodule.h"
#include "widgets/titlelabel.h"

#include <DFontSizeManager>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

TitleModule::TitleModule(const QString &name, const QString &title, QObject *parent)
    : ModuleObject(parent)
{
    setName(name);
    setDescription(title);
    addContentText(title);
}

QWidget *TitleModule::page()
{
    TitleLabel *titleLabel = new TitleLabel(description());
    DFontSizeManager::instance()->bind(titleLabel, DFontSizeManager::T5, QFont::DemiBold); // 设置字体
    return titleLabel;
}
