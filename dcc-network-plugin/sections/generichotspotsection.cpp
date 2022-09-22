// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "generichotspotsection.h"

#include <QLineEdit>

using namespace NetworkManager;
using namespace dcc::widgets;

GenericHotspotSection::GenericHotspotSection(ConnectionSettings::Ptr connSettings, QFrame *parent)
    : AbstractSection(tr("General"), parent)
    , m_connIdItem(new LineEditWidget(this))
    , m_connSettings(connSettings)
{
    initUI();
}

GenericHotspotSection::~GenericHotspotSection()
{
}

bool GenericHotspotSection::allInputValid()
{
    bool valid = true;

    valid = !m_connIdItem->textEdit()->text().isEmpty();

    m_connIdItem->setIsErr(!valid);

    return valid;
}

void GenericHotspotSection::saveSettings()
{
    m_connSettings->setId(m_connIdItem->text());

    // hotspot connection should not be activated automatically by daemon
    m_connSettings->setAutoconnect(false);
}

void GenericHotspotSection::initUI()
{
    m_connIdItem->setTitle(tr("Name"));
    m_connIdItem->setPlaceholderText(tr("Required"));
    m_connIdItem->setText(m_connSettings->id());

    appendItem(m_connIdItem);

    m_connIdItem->textEdit()->installEventFilter(this);
}

bool GenericHotspotSection::eventFilter(QObject *watched, QEvent *event)
{
    // 实现鼠标点击编辑框，确定按钮激活，统一网络模块处理，捕捉FocusIn消息
    if (event->type() == QEvent::FocusIn) {
        if (dynamic_cast<QLineEdit *>(watched))
            Q_EMIT editClicked();
    }

    return QWidget::eventFilter(watched, event);
}
