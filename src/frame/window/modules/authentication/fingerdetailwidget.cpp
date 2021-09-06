#include "fingerdetailwidget.h"
#include "modules/authentication/fingermodel.h"

#include <DApplicationHelper>
#include <DTipLabel>
#include <QBoxLayout>
#include <QLabel>
#include <QScrollArea>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::authentication;
using namespace dcc;
using namespace dcc::authentication;

FingerDetailWidget::FingerDetailWidget(QWidget *parent)
    : QWidget(parent)
    , m_scrollArea(new QScrollArea)
    , m_fingerWidget(new FingerWidget)
{
}

FingerDetailWidget::~FingerDetailWidget()
{

}

void FingerDetailWidget::initFingerUI()
{
    //整体布局
    QVBoxLayout *mainContentLayout = new QVBoxLayout(this);
    mainContentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    m_fingerWidget->setContentsMargins(0, 0, 0, 0);
    m_fingerWidget->layout()->setMargin(0);
    mainContentLayout->addWidget(m_fingerWidget);
    mainContentLayout->addSpacing(30);

    setLayout(mainContentLayout);
    setFocusPolicy(Qt::FocusPolicy::ClickFocus);

    //指纹界面操作
    connect(m_fingerWidget, &FingerWidget::requestAddThumbs, this, &FingerDetailWidget::requestAddThumbs);
    connect(m_fingerWidget, &FingerWidget::requestDeleteFingerItem, this, &FingerDetailWidget::requestDeleteFingerItem);
    connect(m_fingerWidget, &FingerWidget::requestRenameFingerItem, this, &FingerDetailWidget::requestRenameFingerItem);
    connect(m_fingerWidget, &FingerWidget::noticeEnrollCompleted, this, &FingerDetailWidget::noticeEnrollCompleted);
}

void FingerDetailWidget::initNotFingerDevice()
{
    QVBoxLayout *mainContentLayout = new QVBoxLayout(this);
    mainContentLayout->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);

    QLabel *pNotDevice = new QLabel;
    connect(Dtk::Gui::DGuiApplicationHelper::instance(), &Dtk::Gui::DGuiApplicationHelper::themeTypeChanged,
        this, [=](Dtk::Gui::DGuiApplicationHelper::ColorType themeType) {
        Q_UNUSED(themeType);
        pNotDevice->setPixmap(QIcon::fromTheme(getDisplayPath()).pixmap(64, 64));
    });
    pNotDevice->setPixmap(QIcon::fromTheme(getDisplayPath()).pixmap(64, 64));
    pNotDevice->setAlignment(Qt::AlignHCenter);

    DTipLabel *tip = new DTipLabel(tr("No supported devices found"));
    tip->setWordWrap(true);
    tip->setAlignment(Qt::AlignCenter);

    mainContentLayout->addWidget(pNotDevice);
    mainContentLayout->addWidget(tip);
    setLayout(mainContentLayout);
}

QString FingerDetailWidget::getDisplayPath()
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

void FingerDetailWidget::showDeviceStatus(bool hasDevice)
{
    // 指纹设备不支持热插拔 直接显示对应信息
    if (hasDevice) {
        initFingerUI();
    } else {
        initNotFingerDevice();
    }
}

void FingerDetailWidget::setFingerModel(FingerModel *model)
{
    if (!model)
        return;

    m_model = model;
    m_fingerWidget->setFingerModel(model);
    connect(model, &FingerModel::vaildChanged, this, &FingerDetailWidget::showDeviceStatus);
    showDeviceStatus(model->isVaild());
}
