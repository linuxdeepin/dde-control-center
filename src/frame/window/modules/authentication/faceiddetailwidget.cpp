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
    , mainContentLayout(new QVBoxLayout(this))
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
    m_faceWidget->setContentsMargins(0, 0, 0, 0);
    m_faceWidget->layout()->setMargin(0);

    setLayout(mainContentLayout);
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

    m_tip->setWordWrap(true);
    m_tip->setAlignment(Qt::AlignCenter);

    mainContentLayout->addWidget(m_faceWidget);
    mainContentLayout->addWidget(m_pNotDevice);
    mainContentLayout->addWidget(m_tip);
}

void FaceidDetailWidget::onDeviceStatusChanged(bool hasDevice)
{
    if (hasDevice) {
        m_pNotDevice->hide();
        m_tip->hide();
        mainContentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
        m_faceWidget->show();
    } else {
        m_faceWidget->hide();
        mainContentLayout->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
        m_pNotDevice->show();
        m_tip->show();
    }
}
