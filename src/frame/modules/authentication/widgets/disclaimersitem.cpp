#include "disclaimersdialog.h"
#include "disclaimersitem.h"

#include <DApplication>
#include <DSuggestButton>
#include <DCommandLinkButton>
#include <DFontSizeManager>

#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QDebug>

DWIDGET_USE_NAMESPACE

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::authentication;

DisclaimersItem::DisclaimersItem(QWidget *parent)
    : SettingsItem(parent)
    , m_layout(new QHBoxLayout(this))
    , m_acceptCheck(new QCheckBox(this))
{
    m_acceptCheck->setText(tr("I have read and agree to the"));

    m_disclaimersBtn = new DCommandLinkButton(tr("《Disclaimer》"));

    m_layout->setContentsMargins(10, 0, 10, 0);
    m_layout->addStretch();
    m_layout->addWidget(m_acceptCheck, 0, Qt::AlignCenter);
    m_layout->addWidget(m_disclaimersBtn, 0, Qt::AlignCenter);
    m_layout->addStretch();

    DFontSizeManager::instance()->bind(m_acceptCheck, DFontSizeManager::SizeType::T8);
    DFontSizeManager::instance()->bind(m_disclaimersBtn, DFontSizeManager::SizeType::T8);

    connect(m_disclaimersBtn, &QPushButton::clicked, this, &DisclaimersItem::requestSetWindowEnabled);
    connect(m_disclaimersBtn, &QPushButton::clicked, this, &DisclaimersItem::showDisclaimers);
    connect(m_acceptCheck, &QCheckBox::toggled, this, &DisclaimersItem::setAcceptState);
    setLayout(m_layout);
}

void DisclaimersItem::setAcceptState(const bool &state)
{
    m_acceptCheck->setChecked(state);
    Q_EMIT requestStateChange(!state);
}

void DisclaimersItem::showDisclaimers()
{
    DisclaimersDialog *disdlg = new dcc::authentication::DisclaimersDialog(DisclaimersObj::Faceid);
    connect(disdlg, &DisclaimersDialog::requestClickStatus, this, &DisclaimersItem::setAcceptState);
    connect(disdlg, &DisclaimersDialog::requesetCloseDlg, this, &DisclaimersItem::requestSetWindowEnabled);
    disdlg->setWindowFlags(Qt::Dialog | Qt::Popup | Qt::WindowStaysOnTopHint);
    disdlg->setFocus();
    disdlg->activateWindow();
    disdlg->exec();
}

