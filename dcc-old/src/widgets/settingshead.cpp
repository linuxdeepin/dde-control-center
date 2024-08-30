//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "widgets/settingshead.h"
#include "widgets/titlelabel.h"
#include "widgets/accessibleinterface.h"

#include <DCommandLinkButton>

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QApplication>
#include <DFontSizeManager>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;
SET_FORM_ACCESSIBLE(SettingsHead,"SettingsHead");
SettingsHead::SettingsHead(QFrame *parent)
    : SettingsItem(parent)
    , m_title(new TitleLabel)
    , m_edit(new DCommandLinkButton(""))
    , m_state(Cancel)
{
    m_title->setObjectName("SettingsHeadTitle");

    // can not translate correctly just using tr()
    m_edit->setText(tr("Edit"));
    DFontSizeManager::instance()->bind(m_title, DFontSizeManager::T5, QFont::DemiBold);
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 10, 0);
    mainLayout->addWidget(m_title);
    mainLayout->addStretch();
    mainLayout->addWidget(m_edit);
    m_title->setContentsMargins(0, 0, 0, 0);

    setLayout(mainLayout);

    connect(m_edit, &DCommandLinkButton::clicked, this, &SettingsHead::onClicked);
}

void SettingsHead::setTitle(const QString &title)
{
    m_title->setText(title);
    m_edit->setAccessibleName(title);
}

void SettingsHead::setEditEnable(bool state)
{
    m_edit->setVisible(state);
}

void SettingsHead::toEdit()
{
    m_state = Edit;
    refershButton();

    Q_EMIT editChanged(true);
}

void SettingsHead::toCancel()
{
    m_state = Cancel;
    refershButton();

    Q_EMIT editChanged(false);
}

void SettingsHead::onClicked()
{
    if (m_state == Cancel) {
        toEdit();
    } else {
        toCancel();
    }
}

void SettingsHead::refershButton()
{
    if (m_state == Cancel) {
        m_edit->setText(tr("Edit"));
    } else {
        m_edit->setText(tr("Done"));
    }
}
