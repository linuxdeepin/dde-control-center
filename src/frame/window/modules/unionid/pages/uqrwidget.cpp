#include "uqrwidget.h"

#include <QPen>
#include <QBrush>
#include <QPoint>
#include <QPixmap>
#include <QPainter>
#include <QDebug>

const QString REFUSHPATH = ":/themes/light/icons/qrcode_refresh_24px.svg";
UQrFrame::UQrFrame(QWidget *parent)
    : QWidget(parent)
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
    case AshingScanCode:
        initUIAshing();
        break;
    case RefreshScanCode:
        initUIRefresh();
        break;
    case SuccessScanCode:
        initUISuccesss();
        break;
    case QuestionScanCode:
        initUIQuestion();
        break;
    }
}

void UQrFrame::initUI()
{
    m_qrLabel->setFixedSize(158, 158);
    m_qrLabel->setContentsMargins(0, 0, 0, 0);
    opacityEffect->setOpacity(1.0);
    m_qrLabel->setGraphicsEffect(opacityEffect);
    setFixedSize(176, 176);
    QPalette pal(palette());
    pal.setColor(QPalette::Background, Qt::white);
    setPalette(pal);
    m_refreshbutton->hide();
    m_refreshbutton->setFixedSize(68, 68);
    m_refreshbutton->setIconSize(QSize(68, 68));
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

void UQrFrame::initUIAshing()
{
    opacityEffect->setOpacity(0.06);
    m_qrLabel->setGraphicsEffect(opacityEffect);
}

void UQrFrame::initUIRefresh()
{
    opacityEffect->setOpacity(0.06);
    m_qrLabel->setGraphicsEffect(opacityEffect);
    m_refreshbutton->setIcon(QIcon(REFUSHPATH));
    m_refreshbutton->show();
}

void UQrFrame::initUISuccesss()
{
    opacityEffect->setOpacity(0.06);
    m_qrLabel->setGraphicsEffect(opacityEffect);
//    m_refreshbutton->setIcon(QIcon(CHECKPATH));
    m_refreshbutton->show();
}

void UQrFrame::initUIQuestion()
{
    opacityEffect->setOpacity(0.06);
    m_qrLabel->setGraphicsEffect(opacityEffect);
//    m_refreshbutton->setIcon(QIcon(QUESTIONPATH));
    m_refreshbutton->show();
}

/*******************************************************************************
 1. @函数:    showQRcodePicture
 2. @作者:    ut003015 吕亮
 3. @日期:    2021-03-13
 4. @说明:    二维码画图布局
*******************************************************************************/
void UQrFrame::showQRcodePicture(QString url,QSize qrCodeSize,QSize frameSize)
{
    m_qrLabel->setFixedSize(qrCodeSize);
    setFixedSize(frameSize);
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
    QColor penColor = QColor::fromRgbF(0, 0, 0,0.15);
    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setColor(penColor);

    QRect rect = this->rect();
    QRect roundRect = rect.adjusted(+1,+1,-1,-1);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);

    int diameter = 16;
    int cx = 100*diameter/roundRect.width();
    int cy = 100*diameter/roundRect.height();
    painter.save();
    painter.setPen(pen);
    painter.setBrush(Qt::white);
    painter.drawRoundRect(roundRect, cx, cy);
    painter.restore();
    return QWidget::paintEvent(event);
}


