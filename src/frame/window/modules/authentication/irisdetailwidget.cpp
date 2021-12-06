#include "irisdetailwidget.h"
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

IrisDetailWidget::IrisDetailWidget(dcc::authentication::CharaMangerModel *model, QWidget *parent)
    : QWidget (parent)
    , m_model (model)
    , m_mainContentLayout(new QVBoxLayout(this))
    , m_irisWidget(new IrisWidget(model, this))
    , m_pNotDevice(new QLabel(this))
    , m_tip(new DLabel(tr("No supported devices found"), this))
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
    m_mainContentLayout->setContentsMargins(0, 10, 0, 0);

    m_irisWidget->setContentsMargins(0, 0, 0, 0);
    m_irisWidget->layout()->setMargin(0);

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

    m_mainContentLayout->addWidget(m_irisWidget);
    m_mainContentLayout->addWidget(m_pNotDevice);
    m_mainContentLayout->addWidget(m_tip);
}

QString IrisDetailWidget::getDisplayPath()
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

void IrisDetailWidget::onDeviceStatusChanged(bool hasDevice)
{
    if (hasDevice) {
        m_pNotDevice->hide();
        m_tip->hide();
        m_mainContentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
        m_irisWidget->show();
    } else {
        m_irisWidget->hide();
        m_mainContentLayout->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
        m_pNotDevice->show();
        m_tip->show();
    }
}

