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

#include "fingedisclaimer.h"

#include "charamangermodel.h"

#include <DFontSizeManager>
#include <DSuggestButton>
#include <DTitlebar>

#include <QCloseEvent>
#include <QDebug>
#include <QDialog>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

FingerDisclaimer::FingerDisclaimer(QWidget *parent)
    : DAbstractDialog(parent)
    , m_mainLayout(new QVBoxLayout(this))
    , m_fingerPic(new QLabel(this))
    , m_cancelBtn(new QPushButton(this))
    , m_acceptBtn(new DSuggestButton(this))
    , m_currentState(CharaMangerModel::AddInfoState::StartState)
{
    initWidget();
    initConnect();
    QWidget::installEventFilter(this);
}

FingerDisclaimer::~FingerDisclaimer() { }

void FingerDisclaimer::closeEvent(QCloseEvent *event)
{
    Q_EMIT requesetCloseDlg();
    QDialog::closeEvent(event);
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

void FingerDisclaimer::initWidget()
{
    setFixedSize(QSize(382, 446));
    m_mainLayout->setAlignment(Qt::AlignHCenter);

    DTitlebar *titleIcon = new DTitlebar();
    titleIcon->setFrameStyle(QFrame::NoFrame); // 无边框
    titleIcon->setBackgroundTransparent(true); // 透明
    titleIcon->setMenuVisible(false);
    titleIcon->setTitle(tr("Add Fingerprint"));

    m_fingerPic = new QLabel(this);
    m_fingerPic->setPixmap(QIcon::fromTheme(getFacePicture()).pixmap(128, 128));

    // 提示信息
    m_resultTips = new QLabel(this);
    m_resultTips->hide();
    m_explainTips = new DLabel();
    m_explainTips->setWordWrap(true);
    m_explainTips->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(m_explainTips, DFontSizeManager::T8);

    QHBoxLayout *tips = new QHBoxLayout(this);
    tips->addWidget(m_explainTips);
    tips->setContentsMargins(42, 10, 42, 10);

    // 免责声明
    m_disclaimersItem = new DisclaimersItem(DisclaimersObj::Finge, this);
    m_disclaimersItem->show();

    // 下方按钮
    QHBoxLayout *btnLayout = new QHBoxLayout(this);
    m_cancelBtn->setText(tr("Cancel"));
    m_cancelBtn->hide();
    m_acceptBtn->setText(tr("Next"));
    m_acceptBtn->setDisabled(true);

    btnLayout->addWidget(m_cancelBtn, Qt::AlignCenter);
    btnLayout->addSpacing(10);
    btnLayout->addWidget(m_acceptBtn, Qt::AlignHorizontal_Mask);
    btnLayout->setContentsMargins(8, 10, 10, 12);

    m_mainLayout->addWidget(titleIcon, Qt::AlignTop | Qt::AlignRight);
    m_mainLayout->addSpacing(85);
    m_mainLayout->addWidget(m_fingerPic, 0, Qt::AlignHCenter);
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

    this->activateWindow();
    this->setFocus();
}

void FingerDisclaimer::initConnect()
{
    connect(m_disclaimersItem,
            &DisclaimersItem::requestSetWindowEnabled,
            this,
            &FingerDisclaimer::onSetWindowEnabled);
    connect(m_disclaimersItem,
            &DisclaimersItem::requestStateChange,
            m_acceptBtn,
            &QPushButton::setDisabled);
    connect(m_cancelBtn, &QPushButton::clicked, this, &FingerDisclaimer::close);
    connect(m_acceptBtn,
            &QPushButton::clicked,
            this,
            &FingerDisclaimer::requestShowFingeInfoDialog,
            Qt::UniqueConnection);
}

QString FingerDisclaimer::getFacePicture()
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

    return QString(":/icons/deepin/builtin/icons/%1/icons/finger/fingerprint_light.svg").arg(theme);
}

// 处理界面失焦效果 配合 模态对话框
void FingerDisclaimer::onSetWindowEnabled(const bool isEnabled)
{
    this->setEnabled(isEnabled);
}
