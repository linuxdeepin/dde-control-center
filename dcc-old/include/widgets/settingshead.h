//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"
#include "widgets/settingsitem.h"

#include <dtkwidget_global.h>

DWIDGET_BEGIN_NAMESPACE
class DCommandLinkButton;
DWIDGET_END_NAMESPACE

namespace DCC_NAMESPACE {

class TitleLabel;
class SettingsHead : public SettingsItem
{
    Q_OBJECT

public:
    enum State {
        Edit,
        Cancel
    };

public:
    explicit SettingsHead(QFrame *parent = nullptr);

    void setTitle(const QString &title);
    void setEditEnable(bool state = true);

public Q_SLOTS:
    void toEdit();
    void toCancel();

Q_SIGNALS:
    void editChanged(bool edit);

private Q_SLOTS:
    void refershButton();
    void onClicked();

private:
    TitleLabel *m_title;
    DTK_WIDGET_NAMESPACE::DCommandLinkButton *m_edit;

    State m_state;
};

}
