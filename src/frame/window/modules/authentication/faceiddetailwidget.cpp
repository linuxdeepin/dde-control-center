#include "faceiddetailwidget.h"
#include "modules/authentication/charamangermodel.h"

#include <DApplicationHelper>
#include <DTipLabel>
#include <QBoxLayout>
#include <QLabel>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::authentication;
using namespace dcc;
using namespace dcc::authentication;

FaceidDetailWidget::FaceidDetailWidget(dcc::authentication::CharaMangerModel *model, QWidget *parent)
    : QWidget (parent)
    , m_model (model)
    , m_mainContentLayout(new QVBoxLayout(this))
    , m_faceWidget(new FaceWidget(model, this))
    , m_pNotDevice(new QLabel(this))
    , m_tip(new DTipLabel(tr("No supported devices found"), this))
{
    connect(m_model, &CharaMangerModel::vaildFaceDriverChanged, this, &FaceidDetailWidget::onDeviceStatusChanged);
    onDeviceStatusChanged(model->faceDriverVaild());

    initFaceidShow();

    //人脸操作
    connect(m_faceWidget, &FaceWidget::requestAddFace, this, &FaceidDetailWidget::requestAddFace);
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

    m_pNotDevice->setPixmap(QIcon::fromTheme(QString(":/authentication/themes/%1/icons/icon_unknown_device.svg").arg(theme)).pixmap(64, 64));
    m_pNotDevice->setAlignment(Qt::AlignHCenter);

    // 设置高亮字体
    QPalette palette;
    QColor color;
    color.setAlphaF(0.8);
    palette.setColor(QPalette::BrightText, color);

    m_tip->setWordWrap(true);
    m_tip->setAlignment(Qt::AlignCenter);
    m_tip->adjustSize();
    m_tip->setPalette(palette);

    m_mainContentLayout->addWidget(m_faceWidget);
    m_mainContentLayout->addWidget(m_pNotDevice);
    m_mainContentLayout->addWidget(m_tip);
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
