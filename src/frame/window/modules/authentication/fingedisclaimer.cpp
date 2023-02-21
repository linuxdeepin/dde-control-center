// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "fingedisclaimer.h"
#include "modules/authentication/charamangermodel.h"

#include <DTitlebar>
#include <DFontSizeManager>

#include <QVBoxLayout>
#include <QPushButton>
#include <QCloseEvent>
#include <QDialog>

DWIDGET_USE_NAMESPACE

using namespace dcc::authentication;
using namespace DCC_NAMESPACE::authentication;

FingerDisclaimer::FingerDisclaimer(QWidget *parent)
    : BiologicalBaseDialog(parent)
    , m_fingerPic(new QLabel(this))
    , m_resultTips(nullptr)
    , m_explainTips(nullptr)
    , m_currentState(dcc::authentication::CharaMangerModel::AddInfoState::StartState)
{
    initWidget();
    initConnect();
    QWidget::installEventFilter(this);
    setWindowFlags(Qt::WindowStaysOnTopHint | windowFlags());
    setAttribute(Qt::WA_ShowModal, true);
}

FingerDisclaimer::~FingerDisclaimer()
{

}

bool FingerDisclaimer::eventFilter(QObject *o, QEvent *e)
{
    if (o == this && QEvent::WindowDeactivate == e->type()) {
        clearFocus();
        setFocus();
        return true;
    }
    return false;
}

void FingerDisclaimer::initBioWidget()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignHCenter);

    DTitlebar *titleIcon = new DTitlebar(this);
    titleIcon->setFrameStyle(QFrame::NoFrame);//无边框
    titleIcon->setBackgroundTransparent(true);//透明
    titleIcon->setMenuVisible(false);
    titleIcon->setTitle(tr("Add Fingerprint"));

    m_fingerPic = new QLabel(this);
    m_fingerPic->setPixmap(QIcon::fromTheme(getFingerPicture()).pixmap(128, 128));

    // 提示信息
    m_resultTips = new QLabel(this);
    m_resultTips->hide();
    m_explainTips = new DLabel(this);
    m_explainTips->setWordWrap(true);
    m_explainTips->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(m_explainTips, DFontSizeManager::T8);

    QHBoxLayout *tips = new QHBoxLayout();
    tips->addWidget(m_explainTips);
    tips->setContentsMargins(42, 10, 42, 10);

    // 免责声明
    m_disclaimersItem->show();

    // 下方按钮
    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_cancelBtn->hide();
    m_acceptBtn->setText(tr("Next"));
    m_acceptBtn->setDisabled(true);

    btnLayout->addWidget(m_acceptBtn, Qt::AlignHorizontal_Mask);
    btnLayout->setContentsMargins(10, 10, 10, 12);

    mainLayout->addWidget(titleIcon, Qt::AlignTop | Qt::AlignRight);
    mainLayout->addSpacing(85);
    mainLayout->addWidget(m_fingerPic, 0, Qt::AlignHCenter);
    mainLayout->addSpacing(15);
    mainLayout->addWidget(m_resultTips, 0, Qt::AlignHCenter);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(tips);
    mainLayout->addStretch();
    mainLayout->addWidget(m_disclaimersItem, 0, Qt::AlignCenter);
    mainLayout->addLayout(btnLayout);
    mainLayout->setSpacing(0);

    m_bioWidget->setLayout(mainLayout);
}

void FingerDisclaimer::initWidget()
{
    initBioWidget();
    setFixedSize(QSize(382, 446));
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignHCenter);

    mainLayout->addWidget(m_bioWidget);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);
    setDisclaimerVisible(false);

    this->activateWindow();
    this->setFocus();
}

void FingerDisclaimer::initConnect()
{
    connect(m_acceptBtn, &QPushButton::clicked, this, &FingerDisclaimer::requestShowFingeInfoDialog, Qt::UniqueConnection);
}

QString FingerDisclaimer::getFingerPicture()
{
    QString theme;
    QString icon;
    DGuiApplicationHelper::ColorType type = DGuiApplicationHelper::instance()->themeType();
    switch (type) {
    case DGuiApplicationHelper::UnknownType:
        break;
    case DGuiApplicationHelper::LightType:
        theme = QString("light");
        break;
    case DGuiApplicationHelper::DarkType:
        theme = QString("dark");
        break;
    }

    return QString(":/authentication/themes/%1/icons/finger/fingerprint_light.svg").arg(theme);
}
