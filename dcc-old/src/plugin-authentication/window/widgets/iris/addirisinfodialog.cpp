//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "addirisinfodialog.h"
#include "charamangermodel.h"

#include <DTitlebar>
#include <DSuggestButton>
#include <DFontSizeManager>

#include <QCloseEvent>
#include <QBoxLayout>
#include <QTimer>
#include <QPushButton>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

AddIrisInfoDialog::AddIrisInfoDialog(CharaMangerModel *model, QWidget *parent)
    : DAbstractDialog(parent)
    , m_charaModel(model)
    , m_mainLayout(new QVBoxLayout(this))
    , m_irisInfo(new IrisInfoWidget(this))
    , m_resultTips(new QLabel(this))
    , m_explainTips(new QLabel(this))
    , m_disclaimersItem(new DisclaimersItem(DisclaimersObj::Iris, this))
    , m_cancelBtn(new QPushButton(this))
    , m_acceptBtn(new DSuggestButton(this))
{
    initWidget();
    initConnect();
    installEventFilter(this);
}

AddIrisInfoDialog::~AddIrisInfoDialog()
{

}


void AddIrisInfoDialog::closeEvent(QCloseEvent *event)
{
    Q_EMIT requesetCloseDlg();
    if (m_state == CharaMangerModel::AddInfoState::Processing) {
        Q_EMIT requestStopEnroll();
    }
    QDialog::closeEvent(event);
}

bool AddIrisInfoDialog::eventFilter(QObject *o, QEvent *e)
{
    if (o == this && QEvent::WindowDeactivate == e->type()) {
        clearFocus();
        setFocus();
        return true;
    }
    return false;
}

void AddIrisInfoDialog::initWidget()
{
    setFixedSize(QSize(340, 404));
    m_mainLayout->setAlignment(Qt::AlignHCenter);

    DTitlebar *titleIcon = new DTitlebar(this);
    titleIcon->setFrameStyle(QFrame::NoFrame);//无边框
    titleIcon->setBackgroundTransparent(true);//透明
    titleIcon->setMenuVisible(false);
    titleIcon->setTitle(tr("Enroll Iris"));

    // 图片
    m_irisInfo->setFixedSize(QSize(128, 128));
    m_irisInfo->updateState(CharaMangerModel::AddInfoState::StartState);

    // 提示信息
    m_resultTips->hide();
    m_explainTips = new QLabel(this);
    m_explainTips->setWordWrap(true);
    m_explainTips->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(m_explainTips, DFontSizeManager::T8);

    QHBoxLayout *tips = new QHBoxLayout(this);
    tips->addWidget(m_explainTips, 0, Qt::AlignHCenter);

    // 免责声明
    m_disclaimersItem->show();

    // 下方按钮
    QHBoxLayout *btnLayout = new QHBoxLayout(this);
    m_cancelBtn->setText(tr("Cancel"));
    m_cancelBtn->hide();
    m_acceptBtn->setText(tr("Next"));
    m_acceptBtn->setDisabled(true);

    btnLayout->addWidget(m_cancelBtn, Qt::AlignCenter);
    btnLayout->addSpacing(10);
    btnLayout->addWidget(m_acceptBtn, Qt::AlignCenter);
    btnLayout->setContentsMargins(20, 10, 20, 20);

    m_mainLayout->addWidget(titleIcon, Qt::AlignTop | Qt::AlignRight);
    m_mainLayout->addSpacing(30);
    m_mainLayout->addWidget(m_irisInfo, 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(15);
    m_mainLayout->addWidget(m_resultTips, 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(10);
    m_mainLayout->addLayout(tips);
    m_mainLayout->addStretch();
    m_mainLayout->addWidget(m_disclaimersItem, 0, Qt::AlignCenter);
    m_mainLayout->addLayout(btnLayout);

    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    setLayout(m_mainLayout);
}

void AddIrisInfoDialog::initConnect()
{
    connect(m_charaModel, &CharaMangerModel::enrollIrisInfoState, this, &AddIrisInfoDialog::refreshInfoStatusDisplay);
    connect(m_charaModel, &CharaMangerModel::enrollIrisStatusTips, this, &AddIrisInfoDialog::refreshExplainTips);
    connect(m_disclaimersItem, &DisclaimersItem::requestSetWindowEnabled, this, &AddIrisInfoDialog::onSetWindowEnabled);
    connect(m_disclaimersItem, &DisclaimersItem::requestStateChange, m_acceptBtn, &QPushButton::setDisabled);

    connect(m_cancelBtn, &QPushButton::clicked, this, &AddIrisInfoDialog::close);
    connect(m_acceptBtn, &QPushButton::clicked, this, &AddIrisInfoDialog::requestInputIris);
    connect(m_cancelBtn, &QPushButton::clicked, this, [this]{
        if (m_acceptBtn->text() == "Done") {
            this->close();
        }
    });
}

void AddIrisInfoDialog::refreshInfoStatusDisplay(CharaMangerModel::AddInfoState state)
{
    m_irisInfo->updateState(state);
    m_state = state;
    switch (state) {
    case CharaMangerModel::AddInfoState::Processing:{
        m_resultTips->setVisible(false);
        m_disclaimersItem->setVisible(false);
        m_cancelBtn->setVisible(false);
        m_acceptBtn->setVisible(false);
        m_explainTips->setVisible(true);
    }
        break;
    case CharaMangerModel::AddInfoState::Success:{
        m_resultTips->setVisible(true);
        m_resultTips->setText(tr("Iris enrolled"));
        m_disclaimersItem->setVisible(false);
        m_cancelBtn->setVisible(true);
        m_cancelBtn->setText(tr("Done"));
        m_acceptBtn->setVisible(false);
        m_explainTips->setVisible(false);

        Q_EMIT requestStopEnroll();
    }
        break;
    case CharaMangerModel::AddInfoState::Fail:{
        m_resultTips->setVisible(true);
        m_resultTips->setText(tr("Failed to enroll your iris"));
        m_disclaimersItem->setVisible(false);
        m_cancelBtn->setVisible(true);
        m_cancelBtn->setText("Done");
        m_acceptBtn->setVisible(true);
        m_acceptBtn->setText(tr("Try Again"));
        m_explainTips->setVisible(false);

        Q_EMIT requestStopEnroll();
    }
        break;
    default:
        break;
    }
}

void AddIrisInfoDialog::onSetWindowEnabled(const bool isEnabled)
{
    this->setEnabled(isEnabled);
}

void AddIrisInfoDialog::refreshExplainTips(QString tips)
{
    m_explainTips->setText(tips);
}
