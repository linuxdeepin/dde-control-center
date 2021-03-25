#include "uqrwidget.h"
#include "modules/unionid/requestservice.h"

#include <QPen>
#include <QBrush>
#include <QPoint>
#include <QPixmap>
#include <QPainter>
#include <QDebug>


UQrFrame::UQrFrame(QWidget *parent)
    : DFrame(parent)
    , m_qrLabel(new DLabel(this))
    , m_borderwidget(new QWidget(this))
    , m_refreshbutton(new DIconButton(this))
    , m_mainLayout(new QGridLayout(this))
    , opacityEffect(new QGraphicsOpacityEffect(this))
{
    initUI();
    initConnect();
}

/*******************************************************************************
 1. @函数:    setWidgetType
 2. @作者:    ut003015 吕亮
 3. @日期:    2021-03-13
 4. @说明:    设置二维码类型
*******************************************************************************/
void UQrFrame::setWidgetType(qRStyle type)
{
    m_type = type;
    m_refreshbutton->hide();
    initUIType(type);
}

void UQrFrame::initUIType(qRStyle type)
{
    switch (type) {
    case NormalScanCode:
        initUINormal();
        break;
    case TimeoutScanCode:
        initUITimeout();
        break;
    case RefreshScanCode:
        initUIRefresh();
        break;
    case SuccessScanCode:
        initUISuccesss();
        break;
    }
}

void UQrFrame::initUI()
{
    m_qrLabel->setFixedSize(164, 164);
    m_qrLabel->setContentsMargins(0, 0, 0, 0);
    opacityEffect->setOpacity(1.0);
    m_qrLabel->setGraphicsEffect(opacityEffect);
    setFixedSize(176, 176);
    QPalette pal(palette());
    pal.setColor(QPalette::Background, Qt::white);
    setAutoFillBackground(true);
    setPalette(pal);
    m_refreshbutton->hide();
    m_refreshbutton->setFixedSize(48, 48);
    m_refreshbutton->setIconSize(QSize(48, 48));
    m_refreshbutton->setFlat(true);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->addWidget(m_qrLabel, 0, 0, 1, 1, Qt::AlignCenter);
    m_mainLayout->addWidget(m_refreshbutton, 0, 0, 1, 1, Qt::AlignCenter);
    setLayout(m_mainLayout);
}

void UQrFrame::initUINormal()
{
    opacityEffect->setOpacity(1.0);
    m_qrLabel->setGraphicsEffect(opacityEffect);
}

void UQrFrame::initUITimeout()
{
    opacityEffect->setOpacity(0.50);
    m_qrLabel->setGraphicsEffect(opacityEffect);
}

void UQrFrame::initUIRefresh()
{
    opacityEffect->setOpacity(0.10);
    m_qrLabel->setGraphicsEffect(opacityEffect);
    m_refreshbutton->setIcon(QIcon(":/themes/light/icons/qrcode_refresh_24px.svg"));
    m_refreshbutton->show();
}

void UQrFrame::initUISuccesss()
{
    opacityEffect->setOpacity(0.10);
    m_qrLabel->setGraphicsEffect(opacityEffect);
//    m_refreshbutton->setIcon(QIcon(CHECKPATH));
    m_refreshbutton->show();
}

/*******************************************************************************
 1. @函数:    showQRcodePicture
 2. @作者:    ut003015 吕亮
 3. @日期:    2021-03-13
 4. @说明:    二维码画图布局
*******************************************************************************/
void UQrFrame::showQRcodePicture(QString url)
{
    std::vector<QrSegment> segs = QrSegment::makeSegments(url.toLatin1());
    QrCode qr1 = QrCode::encodeSegments(segs, QrCode::Ecc::HIGH, 5, 40, 2, false);
    //创建二维码画布
    QImage QrCode_Image = QImage(qr1.getSize(), qr1.getSize(), QImage::Format_RGB888);

    for (int y = 0; y < qr1.getSize(); y++) {
        for (int x = 0; x < qr1.getSize(); x++) {
            if (qr1.getModule(x, y) == 0)
                QrCode_Image.setPixel(x, y, qRgb(255, 255, 255));
            else
                QrCode_Image.setPixel(x, y, qRgb(0, 0, 0));
        }
    }

    //图像大小转换为适当的大小
    QrCode_Image = QrCode_Image.scaled(m_qrLabel->width(), m_qrLabel->height(),
                                       Qt::KeepAspectRatio);
    //转换为QPixmap在Label中显示
    m_qrLabel->setPixmap(QPixmap::fromImage(QrCode_Image));

}

DIconButton *UQrFrame::getFreshButton()
{
    return m_refreshbutton;
}

void UQrFrame::setOpacity(qreal opacity)
{
    opacityEffect->setOpacity(opacity);
    m_qrLabel->setGraphicsEffect(opacityEffect);
}

void UQrFrame::initConnect()
{
    connect(m_refreshbutton, &DIconButton::clicked, this, &UQrFrame::refreshsignal);
}

void UQrFrame::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPen pen;
    pen.setColor(QColor(0, 0, 0, 100));
//    QRect r = rect();
//    painter.drawRoundRect(r, 5, 5);
}


