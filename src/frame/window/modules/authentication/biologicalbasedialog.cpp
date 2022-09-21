// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "biologicalbasedialog.h"

#include <QHBoxLayout>

DWIDGET_USE_NAMESPACE
using namespace dcc::authentication;

namespace DCC_NAMESPACE {
namespace authentication {

BiologicalBaseDialog::BiologicalBaseDialog(QWidget *parent)
    : DAbstractDialog(parent)
    , m_bioWidget(new QWidget(this))
    , m_disclaimerWidget(new DisclaimersDialog(DisclaimersObj::Finger, this))
    , m_disclaimersItem(new DisclaimersItem(m_bioWidget))
    , m_cancelBtn(new QPushButton(m_bioWidget))
    , m_acceptBtn(new DSuggestButton(m_bioWidget))
{
    connect(m_disclaimersItem, &DisclaimersItem::requestShowDisclaimers, this, &BiologicalBaseDialog::onShowDisclaimer);
    connect(m_disclaimersItem, &DisclaimersItem::requestStateChange, m_acceptBtn, [this] (bool state) {
        const bool enable = !state;
        m_acceptBtn->setEnabled(enable);
        if (enable) {
            m_acceptBtn->setFocus();
        } else {
            // 不明确设置的话，焦点可能会在关闭按钮上
            m_disclaimersItem->setFocus();
        }
    });
    connect(m_disclaimerWidget, &DisclaimersDialog::acceptDisclaimer, this, &BiologicalBaseDialog::onAcceptDisclaimer);
    connect(this, &QDialog::finished, this, [this] {
        setDisclaimerVisible(false);
    });
}

BiologicalBaseDialog::~BiologicalBaseDialog()
{

}

void BiologicalBaseDialog::onShowDisclaimer()
{
    setDisclaimerVisible(true);
    layout()->replaceWidget(m_bioWidget, m_disclaimerWidget);
}

void BiologicalBaseDialog::onAcceptDisclaimer(bool accept)
{
    setDisclaimerVisible(false);
    layout()->replaceWidget(m_disclaimerWidget, m_bioWidget);
    m_disclaimersItem->setAcceptState(accept);
}

void BiologicalBaseDialog::setDisclaimerVisible(bool visible)
{
    m_bioWidget->setVisible(!visible);
    m_disclaimerWidget->setVisible(visible);
}

}
}
