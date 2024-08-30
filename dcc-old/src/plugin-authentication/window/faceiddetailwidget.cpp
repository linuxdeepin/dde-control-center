//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "faceiddetailwidget.h"
#include "widgets/face/faceinfodialog.h"
#include "widgets/face/addfaceinfodialog.h"
#include "charamangermodel.h"

#include <DGuiApplicationHelper>
#include <DFontSizeManager>
#include <DTipLabel>
#include <DIconTheme>

#include <QBoxLayout>
#include <QLabel>

DGUI_USE_NAMESPACE

FaceidDetailWidget::FaceidDetailWidget(CharaMangerModel *model, QWidget *parent)
    : QWidget (parent)
    , m_model (model)
    , m_mainContentLayout(new QVBoxLayout(this))
    , m_faceWidget(new FaceWidget(model, this))
    , m_pNotDevice(new QLabel(this))
    , m_tip(new DLabel(tr("No supported devices found"), this))
    , m_addFaceInfodlg(new AddFaceInfoDialog(model, this))
    , m_facedlg(new FaceInfoDialog(model, this))
{
    connect(m_model, &CharaMangerModel::vaildFaceDriverChanged, this, &FaceidDetailWidget::onDeviceStatusChanged);
    onDeviceStatusChanged(model->faceDriverVaild());

    initFaceidShow();

    //人脸操作
    connect(m_faceWidget, &FaceWidget::requestAddFace, this, &FaceidDetailWidget::onShowAddFaceDialog);
    connect(m_faceWidget, &FaceWidget::requestDeleteFaceItem, this, &FaceidDetailWidget::requestDeleteFaceItem);
    connect(m_faceWidget, &FaceWidget::requestRenameFaceItem, this, &FaceidDetailWidget::requestRenameFaceItem);
    connect(m_faceWidget, &FaceWidget::noticeEnrollCompleted, this, &FaceidDetailWidget::noticeEnrollCompleted);
}

FaceidDetailWidget::~FaceidDetailWidget()
{

}

void FaceidDetailWidget::initFaceidShow()
{
    //整体布局
    m_mainContentLayout->setContentsMargins(0, 10, 0, 0);

    m_faceWidget->setContentsMargins(0, 0, 0, 0);
    m_faceWidget->layout()->setMargin(0);

    setLayout(m_mainContentLayout);
    setFocusPolicy(Qt::FocusPolicy::ClickFocus);

    connect(Dtk::Gui::DGuiApplicationHelper::instance(), &Dtk::Gui::DGuiApplicationHelper::themeTypeChanged,
        this, [=](Dtk::Gui::DGuiApplicationHelper::ColorType themeType) {
        Q_UNUSED(themeType);
        m_pNotDevice->setPixmap(DIconTheme::findQIcon(getDisplayPath()).pixmap(64, 64));
    });

    m_pNotDevice->setPixmap(DIconTheme::findQIcon(getDisplayPath()).pixmap(64, 64));
    m_pNotDevice->setAlignment(Qt::AlignHCenter);

    // 设置高亮字体
    m_tip->setEnabled(false);
    auto pal = m_tip->palette();
    DFontSizeManager::instance()->bind(m_tip, DFontSizeManager::T7);
    QColor base_color = pal.text().color();
    base_color.setAlpha(255 / 10 * 2);
    pal.setColor(QPalette::Text, base_color);
    m_tip->setPalette(pal);

    m_mainContentLayout->addWidget(m_faceWidget);
    m_mainContentLayout->addWidget(m_pNotDevice);
    m_mainContentLayout->addWidget(m_tip);
}

QString FaceidDetailWidget::getDisplayPath()
{
    QString theme;
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
    return QString(":/authentication/themes/%1/icons/icon_unknown_device.svg").arg(theme);
}


void FaceidDetailWidget::onDeviceStatusChanged(bool hasDevice)
{
    if (hasDevice) {
        m_pNotDevice->hide();
        m_tip->hide();
        m_mainContentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
        m_faceWidget->show();
    } else {
        m_faceWidget->hide();
        m_mainContentLayout->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
        m_pNotDevice->show();
        m_tip->show();
    }
}

void FaceidDetailWidget::onShowAddFaceDialog(const QString &driverName, const int &charaType, const QString &charaName)
{
    m_addFaceInfodlg->responseEnrollInfoState(CharaMangerModel::AddInfoState::StartState, QString());

    connect(m_addFaceInfodlg, &AddFaceInfoDialog::requestStopEnroll, this, &FaceidDetailWidget::requestStopEnroll, Qt::UniqueConnection);

    // 用户点击对话框开始录入
    disconnect(m_addFaceInfodlg, &AddFaceInfoDialog::requestShowFaceInfoDialog, this, nullptr);
    connect(m_addFaceInfodlg, &AddFaceInfoDialog::requestShowFaceInfoDialog, this, [=](){
        m_addFaceInfodlg->hide();
        onShowAddFaceidVideo();
        Q_EMIT requestEntollStart(driverName, charaType, charaName);
    });

    m_addFaceInfodlg->setWindowFlags(Qt::Dialog | Qt::Popup | Qt::WindowStaysOnTopHint);
    m_addFaceInfodlg->show();
    m_addFaceInfodlg->setFocus();
    m_addFaceInfodlg->activateWindow();
}

void FaceidDetailWidget::onConnectFD(const int &facedf)
{
    m_facedlg->faceInfoLabel()->createConnection(facedf);

    m_facedlg->setWindowFlags(Qt::Dialog | Qt::Popup | Qt::WindowStaysOnTopHint);
    m_facedlg->show();
    m_facedlg->setFocus();
    m_facedlg->activateWindow();
}

void FaceidDetailWidget::onShowAddFaceidVideo()
{
    // 开始录入人脸
    connect(m_facedlg, &FaceInfoDialog::requestCloseDlg, this, &FaceidDetailWidget::requestStopEnroll, Qt::UniqueConnection);

    // 开始录入就弹出  TODO:  处理拿到FD后的内容
    connect(m_model, &CharaMangerModel::tryStartInputFace, this, &FaceidDetailWidget::onConnectFD, Qt::UniqueConnection);
}
