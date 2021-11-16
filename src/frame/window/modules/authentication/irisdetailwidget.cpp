#include "irisdetailwidget.h"
#include "modules/authentication/charamangermodel.h"

#include <DApplicationHelper>
#include <DTipLabel>
#include <QBoxLayout>
#include <QLabel>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::authentication;
using namespace dcc;
using namespace dcc::authentication;

IrisDetailWidget::IrisDetailWidget(dcc::authentication::CharaMangerModel *model, QWidget *parent)
    : QWidget (parent)
    , m_model (model)
    , mainContentLayout(new QVBoxLayout(this))
    , m_irisWidget(new IrisWidget(model, this))
    , m_pNotDevice(new QLabel(this))
    , m_tip(new DTipLabel(tr("No supported devices found"), this))
{
    connect(m_model, &CharaMangerModel::vaildIrisDriverChanged, this, &IrisDetailWidget::onDeviceStatusChanged);
    onDeviceStatusChanged(model->irisDriverVaild());

    initIrisShow();

    //人脸操作
    connect(m_irisWidget, &IrisWidget::requestAddIris, this, &IrisDetailWidget::requestAddIris);
    connect(m_irisWidget, &IrisWidget::requestDeleteIrisItem, this, &IrisDetailWidget::requestDeleteIrisItem);
    connect(m_irisWidget, &IrisWidget::requestRenameIrisItem, this, &IrisDetailWidget::requestRenameIrisItem);
    connect(m_irisWidget, &IrisWidget::noticeEnrollCompleted, this, &IrisDetailWidget::noticeEnrollCompleted);
}

IrisDetailWidget::~IrisDetailWidget()
{

}

void IrisDetailWidget::initIrisShow()
{
    //整体布局
    m_irisWidget->setContentsMargins(0, 0, 0, 0);
    m_irisWidget->layout()->setMargin(0);

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

    QPalette palette;
    QColor color;
    color.setAlphaF(0.8);
    palette.setColor(QPalette::BrightText, color);

    m_tip->setWordWrap(true);
    m_tip->setAlignment(Qt::AlignCenter);
    m_tip->adjustSize();
    m_tip->setPalette(palette);

    mainContentLayout->addWidget(m_irisWidget);
    mainContentLayout->addWidget(m_pNotDevice);
    mainContentLayout->addWidget(m_tip);
}

void IrisDetailWidget::onDeviceStatusChanged(bool hasDevice)
{
    if (hasDevice) {
        m_pNotDevice->hide();
        m_tip->hide();
        mainContentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
        m_irisWidget->show();
    } else {
        m_irisWidget->hide();
        mainContentLayout->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
        m_pNotDevice->show();
        m_tip->show();
    }
}

