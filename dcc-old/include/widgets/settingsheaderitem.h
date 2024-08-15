//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"
#include "widgets/settingsitem.h"

class QHBoxLayout;

namespace DCC_NAMESPACE {

class TitleLabel;
class SettingsHeaderItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit SettingsHeaderItem(QWidget *parent = 0);

    TitleLabel *textLabel() const { return m_headerText; }
    QHBoxLayout *layout() const { return m_mainLayout; }

    void setTitle(const QString &title);
    void setRightWidget(QWidget *widget);

private:
    QHBoxLayout *m_mainLayout;
    TitleLabel *m_headerText;
};

}
