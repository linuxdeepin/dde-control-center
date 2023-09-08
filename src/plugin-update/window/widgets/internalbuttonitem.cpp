// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "internalbuttonitem.h"

#include <QBoxLayout>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>

InternalButtonItem::InternalButtonItem(QWidget *parent)
    : SettingsItem(parent)
    , m_internalLabel(new QLabel(tr("Internal testing channel"), this))
    , m_switchbtn(new DSwitchButton(this))
    , m_commandlink(new DCommandLinkButton(tr("click here open the link"), this))
{
    initUi();
    initConnection();
}

void InternalButtonItem::setLink(const QUrl &link)
{
    m_link = link;
}

void InternalButtonItem::initUi()
{
    QHBoxLayout *mainlayout = new QHBoxLayout(this);
    mainlayout->setSpacing(0);
    mainlayout->setContentsMargins(10, 1, 10, 1);
    mainlayout->addWidget(m_internalLabel);
    mainlayout->addStretch();
    mainlayout->addWidget(m_commandlink);
    mainlayout->addWidget(m_switchbtn);

    m_commandlink->setVisible(false);
}

void InternalButtonItem::initConnection()
{
    connect(m_commandlink, &DCommandLinkButton::clicked, this, [this] {
        QDesktopServices::openUrl(m_link);
    });
    connect(m_switchbtn, &DSwitchButton::checkedChanged, this, [this](auto value) {
        if (m_switchbtn->isEnabled()) {
            m_switchbtn->setEnabled(false);
            emit InternalButtonItem::requestInternalChannel(value);
        }
    });
}

void InternalButtonItem::onModelTestingStatusChanged(const TestingChannelStatus &status)
{
    switch (status) {
    case TestingChannelStatus::NotJoined:
        m_commandlink->hide();
        m_switchbtn->setChecked(false);
        m_switchbtn->setEnabled(true);
        break;
    case TestingChannelStatus::WaitJoined:
        m_switchbtn->setChecked(false);
        m_commandlink->show();
        m_switchbtn->setEnabled(false);
        break;
    case TestingChannelStatus::WaitToLeave:
        m_switchbtn->setChecked(true);
        m_commandlink->hide();
        m_switchbtn->setEnabled(false);
        break;
    case TestingChannelStatus::Joined:
        m_commandlink->hide();
        m_switchbtn->setChecked(true);
        m_switchbtn->setEnabled(true);
        break;
    case TestingChannelStatus::DeActive:
        m_switchbtn->setChecked(false);
        m_switchbtn->setEnabled(true);
        break;
    default:
        break;
    }
}
