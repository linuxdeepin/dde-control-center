#include "faceiddetailwidget.h"
#include "modules/authentication/charamangermodel.h"

#include <DApplicationHelper>
#include <DFontSizeManager>
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
    , m_tip(new DLabel(tr("No supported devices found"), this))
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

    connect(Dtk::Gui::DGuiApplicationHelper::instance(), &Dtk::Gui::DGuiApplicationHelper::themeTypeChanged,
        this, [=](Dtk::Gui::DGuiApplicationHelper::ColorType themeType) {
        Q_UNUSED(themeType);
        m_pNotDevice->setPixmap(QIcon::fromTheme(getDisplayPath()).pixmap(64, 64));
    });

    m_pNotDevice->setPixmap(QIcon::fromTheme(getDisplayPath()).pixmap(64, 64));
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
