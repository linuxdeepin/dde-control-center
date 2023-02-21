// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "addfaceinfodialog.h"
#include "modules/authentication/charamangermodel.h"

#include <DTitlebar>
#include <DFontSizeManager>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCloseEvent>
#include <QTimer>
#include <QDebug>
#include <QDialog>

DWIDGET_USE_NAMESPACE

using namespace dcc::authentication;
using namespace DCC_NAMESPACE::authentication;

AddFaceInfoDialog::AddFaceInfoDialog(dcc::authentication::CharaMangerModel *model, QWidget *parent)
    : BiologicalBaseDialog(parent)
    , m_faceModel(model)
    , m_spaceWidget(new QWidget(this))
    , m_currentState(dcc::authentication::CharaMangerModel::AddInfoState::StartState)
{
    initWidget();
    initConnect();
    QWidget::installEventFilter(this);
    setAttribute(Qt::WA_ShowModal, true);
    // 此处设置焦点代理是取巧了，如果没有m_acceptBtn没有显示，则焦点下移到cancel按钮上，见bug133453
    setFocusProxy(m_acceptBtn);
}

AddFaceInfoDialog::~AddFaceInfoDialog()
{

}

void AddFaceInfoDialog::closeEvent(QCloseEvent *event)
{
    Q_EMIT requestCloseDlg();
    m_faceModel->setAddButtonStatus(true);
    BiologicalBaseDialog::closeEvent(event);
}

bool AddFaceInfoDialog::eventFilter(QObject *o, QEvent *e)
{
    if (o == this && QEvent::WindowDeactivate == e->type()) {
        clearFocus();
        setFocus();
        return true;
    }
    return false;
}

void AddFaceInfoDialog::initBioWidget()
{
    QVBoxLayout *bioLayout = new QVBoxLayout(this);
    bioLayout->setAlignment(Qt::AlignHCenter);

    DTitlebar *titleIcon = new DTitlebar();
    titleIcon->setFrameStyle(QFrame::NoFrame);//无边框
    titleIcon->setBackgroundTransparent(true);//透明
    titleIcon->setMenuVisible(false);
    titleIcon->setTitle(tr("Enroll Face"));

    // 人脸图片
    m_facePic = new QLabel(this);
    m_facePic->setPixmap(QIcon::fromTheme(getFacePicture()).pixmap(128,128));

    // 提示信息
    m_resultTips = new QLabel(this);
    m_resultTips->hide();
    m_explainTips = new DLabel(tr("Make sure all parts of your face are not covered by objects and are clearly visible. Your face should be well-lit as well."));
    m_explainTips->setWordWrap(true);
    m_explainTips->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(m_explainTips, DFontSizeManager::T8);

    QHBoxLayout *tips = new QHBoxLayout(this);
    tips->addWidget(m_explainTips);
    tips->setContentsMargins(42, 10, 42, 10);

    // 免责声明
    m_disclaimersItem->show();

    m_cancelBtn->setText(tr("Cancel"));
    m_cancelBtn->hide();
    m_spaceWidget->setFixedWidth(10);
    m_spaceWidget->hide();
    m_acceptBtn->setText(tr("Next"));
    m_acceptBtn->setDisabled(true);

    // 下方按钮
    QHBoxLayout *btnLayout = new QHBoxLayout(this);
    btnLayout->addWidget(m_cancelBtn, Qt::AlignCenter);
    btnLayout->addWidget(m_spaceWidget);
    btnLayout->addWidget(m_acceptBtn, Qt::AlignCenter);
    btnLayout->setContentsMargins(20, 10, 20, 20);

    bioLayout->addWidget(titleIcon, Qt::AlignTop | Qt::AlignRight);
    bioLayout->addSpacing(55);
    bioLayout->addWidget(m_facePic, 0, Qt::AlignHCenter);
    bioLayout->addSpacing(15);
    bioLayout->addWidget(m_resultTips, 0, Qt::AlignHCenter);
    bioLayout->addSpacing(10);
    bioLayout->addLayout(tips);
    bioLayout->addStretch();
    bioLayout->addWidget(m_disclaimersItem, 0, Qt::AlignCenter);
    bioLayout->addLayout(btnLayout);
    bioLayout->setSpacing(0);

    m_bioWidget->setLayout(bioLayout);
}

void AddFaceInfoDialog::initWidget()
{
    initBioWidget();
    setFixedSize(QSize(454, 542));
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

void AddFaceInfoDialog::initConnect()
{
    connect(m_faceModel, &CharaMangerModel::enrollInfoState, this, &AddFaceInfoDialog::responseEnrollInfoState);
    connect(m_cancelBtn, &QPushButton::clicked, this, &AddFaceInfoDialog::close);
    connect(m_acceptBtn, &QPushButton::clicked, this, [this](){
        m_faceModel->setAddButtonStatus(false);
        Q_EMIT requestShowFaceInfoDialog();
    });
}

QString AddFaceInfoDialog::getFacePicture()
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

    switch (m_currentState) {
    case dcc::authentication::CharaMangerModel::AddInfoState::StartState:
        icon = QString("start");
        break;
    case dcc::authentication::CharaMangerModel::AddInfoState::Success:
        icon = QString("success");
        break;
    case dcc::authentication::CharaMangerModel::AddInfoState::Fail:
        icon = QString("fail");
        break;
    default:
        break;
    }

    return QString(":/authentication/themes/%1/icons/icon_face-%2.svg").arg(theme).arg(icon);
}

void AddFaceInfoDialog::responseEnrollInfoState(CharaMangerModel::AddInfoState state, const QString &tips)
{
    m_currentState = state;

    m_facePic->setPixmap(QIcon::fromTheme(getFacePicture()).pixmap(128, 128));
    if (m_currentState == CharaMangerModel::AddInfoState::StartState) {
        m_resultTips->hide();

        m_explainTips->setText(tr("Make sure all parts of your face are not covered by objects and are clearly visible. Your face should be well-lit as well."));
        m_disclaimersItem->setAcceptState(false);
        m_disclaimersItem->show();

        m_cancelBtn->hide();
        m_spaceWidget->hide();
        m_acceptBtn->setText(tr("Next"));
        m_acceptBtn->setDisabled(true);
        m_acceptBtn->show();
        m_acceptBtn->setFocus();
        return;
    }

    if (m_currentState == CharaMangerModel::AddInfoState::Success) {
        m_resultTips->setText(tr("Face enrolled"));
        m_resultTips->show();

        m_explainTips->setText(tr("Use your face to unlock the device and make settings later"));

        m_disclaimersItem->hide();
        m_acceptBtn->hide();
        m_cancelBtn->show();
        m_spaceWidget->hide();
        m_cancelBtn->setFocus();
        m_cancelBtn->setText(tr("Done"));
    }

    if (m_currentState == CharaMangerModel::AddInfoState::Fail) {
        m_resultTips->setText(tr("Failed to enroll your face"));
        m_resultTips->show();

        m_explainTips->setText(tips);

        m_disclaimersItem->hide();
        m_acceptBtn->show();
        m_acceptBtn->setText(tr("Try Again"));
        m_acceptBtn->setDisabled(false);
        m_acceptBtn->setFocus();
        m_cancelBtn->show();
        m_spaceWidget->show();

        m_cancelBtn->setText(tr("Close"));
    }

    this->show();
    Q_EMIT requestStopEnroll();
}
