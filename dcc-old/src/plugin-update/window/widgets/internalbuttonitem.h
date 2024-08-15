// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "widgets/settingsitem.h"
#include "common.h"

#include <DCommandLinkButton>
#include <DSwitchButton>

#include <QLabel>
#include <QUrl>

using DCC_NAMESPACE::SettingsItem;
using Dtk::Widget::DCommandLinkButton;
using Dtk::Widget::DSwitchButton;

class InternalButtonItem final : public SettingsItem
{
    Q_OBJECT

public:
    explicit InternalButtonItem(QWidget *parent = nullptr);

public slots:
    void setLink(const QUrl &link);
    void onModelTestingStatusChanged(const TestingChannelStatus &status);

signals:
    void requestInternalChannel(bool);

private:
    void initUi();
    void initConnection();

private:
    QLabel *m_internalLabel;
    DSwitchButton *m_switchbtn;
    DCommandLinkButton *m_commandlink;
    QUrl m_link;
};
