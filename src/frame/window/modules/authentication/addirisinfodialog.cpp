/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "addirisinfodialog.h"
#include "modules/authentication/charamangermodel.h"

#include <DTitlebar>
#include <DFontSizeManager>

#include <QCloseEvent>
#include <QBoxLayout>
#include <QTimer>

DWIDGET_USE_NAMESPACE

using namespace dcc::authentication;
using namespace DCC_NAMESPACE::authentication;

AddIrisInfoDialog::AddIrisInfoDialog(CharaMangerModel *model, DAbstractDialog *parent)
    : DAbstractDialog(parent)
    , m_charaModel(model)
    , m_mainLayout(new QVBoxLayout(this))
    , m_irisInfo(new IrisInfoWidget(this))
    , m_resultTips(new QLabel(this))
    , m_explainTips(new QLabel(this))
    , m_disclaimersItem(new DisclaimersItem(this))
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
    setFixedSize(QSize(382, 446));
    m_mainLayout->setAlignment(Qt::AlignHCenter);

    DTitlebar *titleIcon = new DTitlebar(this);
    titleIcon->setFrameStyle(QFrame::NoFrame);//无边框
    titleIcon->setBackgroundTransparent(true);//透明
    titleIcon->setMenuVisible(false);
    titleIcon->setTitle(tr("Enroll Iris"));

    // 图片
    m_irisInfo->setFixedSize(QSize(150, 150));
    m_irisInfo->updateState(CharaMangerModel::AddInfoState::StartState);

    // 提示信息
    m_resultTips->hide();
    m_explainTips = new QLabel(tr("Look at the iris scanner and make sure your eyes are inside the frame"));
    m_explainTips->setWordWrap(true);
    m_explainTips->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(m_explainTips, DFontSizeManager::T9);

    QHBoxLayout *tips = new QHBoxLayout(this);
    tips->addWidget(m_explainTips, 0, Qt::AlignHCenter);
//    tips->setContentsMargins(42, 10, 42, 10);

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
    m_mainLayout->addSpacing(25);
    m_mainLayout->addWidget(m_irisInfo, 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(15);
    m_mainLayout->addWidget(m_resultTips, 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(10);
    m_mainLayout->addLayout(tips);
    m_mainLayout->addSpacing(100);
    m_mainLayout->addWidget(m_disclaimersItem, 0, Qt::AlignCenter);
    m_mainLayout->addLayout(btnLayout);

    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    setLayout(m_mainLayout);
}

void AddIrisInfoDialog::initConnect()
{
    connect(m_charaModel, &CharaMangerModel::enrollIrisInfoState, this, &AddIrisInfoDialog::refreshInfoStatusDisplay);
    connect(m_disclaimersItem, &DisclaimersItem::requestSetWindowEnabled, this, &AddIrisInfoDialog::onSetWindowEnabled);
    connect(m_disclaimersItem, &DisclaimersItem::requestStateChange, m_acceptBtn, &QPushButton::setDisabled);

    connect(m_cancelBtn, &QPushButton::clicked, this, &AddIrisInfoDialog::close);
    connect(m_acceptBtn, &QPushButton::clicked, this, &AddIrisInfoDialog::requestInputIris);
    connect(m_acceptBtn, &QPushButton::clicked, this, [this]{
        if (m_acceptBtn->text() == "Done") {
            this->close();
        }
    });
}

void AddIrisInfoDialog::refreshInfoStatusDisplay(CharaMangerModel::AddInfoState state)
{
    m_irisInfo->updateState(state);
    switch (state) {
    case CharaMangerModel::AddInfoState::Processing:{
        m_resultTips->setVisible(false);
        m_disclaimersItem->setVisible(false);
        m_cancelBtn->setVisible(false);
        m_acceptBtn->setVisible(false);
    }
        break;
    case CharaMangerModel::AddInfoState::Success:{
        m_resultTips->setVisible(true);
        m_resultTips->setText(tr("Iris enrolled"));
        m_disclaimersItem->setVisible(false);
        m_cancelBtn->setVisible(true);
        m_cancelBtn->setText(tr("Close"));
        m_acceptBtn->setVisible(true);
        m_cancelBtn->setText(tr("Try Again"));
    }
        break;
    case CharaMangerModel::AddInfoState::Fail:{
        m_resultTips->setVisible(true);
        m_resultTips->setText(tr("Failed to enroll your iris"));
        m_disclaimersItem->setVisible(false);
        m_acceptBtn->setVisible(true);
        m_cancelBtn->setText("enrolled");
    }
        break;
    }
}

void AddIrisInfoDialog::onSetWindowEnabled(const bool isEnabled)
{
    this->setEnabled(isEnabled);
}
