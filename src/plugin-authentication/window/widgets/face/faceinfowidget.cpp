//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "faceinfowidget.h"

#include <DPlatformTheme>

#include <QDBusUnixFileDescriptor>
#include <QBoxLayout>
#include <QTimer>
#include <QPainter>
#include <QPainterPath>
#include <DGuiApplicationHelper>

using Dtk::Gui::DGuiApplicationHelper;

#define Faceimg_SIZE 248

FaceInfoWidget::FaceInfoWidget(QWidget *parent)
    : QLabel (parent)
    , m_faceLable(new QLabel(this))
    , m_startTimer(new QTimer(this))
    , m_themeColor(DGuiApplicationHelper::instance()->systemTheme()->activeColor())
    , m_persent(0)
    , m_rotateAngle(0)
{
    initWidget();

    connect(m_startTimer, &QTimer::timeout, this, &FaceInfoWidget::onUpdateProgressbar);
    m_startTimer->start(100);
}

FaceInfoWidget::~FaceInfoWidget()
{
    if (m_startTimer)
        m_startTimer->stop();
    m_faceLable = nullptr;
}

void FaceInfoWidget::initWidget()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setFixedSize(QSize(258, 258));
    mainLayout->setAlignment(Qt::AlignHCenter);

    mainLayout->addWidget(m_faceLable, 0, Qt::AlignHCenter);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);
}

void FaceInfoWidget::createConnection(const int fd)
{
    m_faceLable->setPixmap(QPixmap());
    DA_read_frames(fd, static_cast<void *>(m_faceLable), recvCamara);
}

void FaceInfoWidget::onUpdateProgressbar()
{
    m_persent >= 96 ? m_persent = 0 : m_persent += 4;
    update();
}

void FaceInfoWidget::recvCamara(void *const context, const DA_img *const img)
{
    if (!context)
        return;

    QLabel *label_ptr = static_cast<QLabel *>(context);

    QPixmap pix(Faceimg_SIZE, Faceimg_SIZE);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QPainterPath path;
    path.addEllipse(0, 0, Faceimg_SIZE, Faceimg_SIZE);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, Faceimg_SIZE, Faceimg_SIZE, QPixmap::fromImage(QImage((uchar *)(img->data), img->width, img->height, QImage::Format_RGB888)));
    if (label_ptr) {
        label_ptr->setPixmap(pix);
    }
}


void FaceInfoWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.save();

    m_rotateAngle = 360 * m_persent / 100;
    int side = qMin(width(), height());
    QRectF outRect(0, 0, side, side);
    QRectF inRect(5, 5, side-10, side-10);

    painter.setPen(Qt::NoPen);
    painter.setOpacity(0.1);
    switch (DGuiApplicationHelper::instance()->themeType()) {
    case DGuiApplicationHelper::UnknownType:
        break;
    case DGuiApplicationHelper::LightType:
        painter.setBrush(QBrush(QColor("#000000")));
        break;
    case DGuiApplicationHelper::DarkType:
        painter.setBrush(QBrush(QColor("#ffffff")));
        break;
    }
    painter.drawEllipse(outRect);

    painter.setOpacity(1);
    painter.setBrush(QBrush(m_themeColor));
    // startAngle和spanAngle必须以1/16度指定，即整圆等于5760（16 * 360）
    painter.drawPie(outRect, (90 - m_rotateAngle)*16, 40 * 16);

    //画遮罩
    painter.setBrush(palette().window().color());
    painter.drawEllipse(inRect);
    painter.restore();

    QWidget::paintEvent(event);
}
