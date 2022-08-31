// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "widgets/optionlistpage.h"

using namespace dcc::widgets;

OptionListPage::OptionListPage(const QStringList &options, QWidget *parent)
    : ContentWidget(parent),

      m_options(options)
{

}
