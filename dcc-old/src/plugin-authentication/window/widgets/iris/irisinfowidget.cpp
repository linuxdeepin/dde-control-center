//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "irisinfowidget.h"

#include <DGuiApplicationHelper>
#include <QHBoxLayout>
#include <QPainter>
#include <QTimer>

DGUI_USE_NAMESPACE

IrisInfoWidget::IrisInfoWidget(QWidget *parent)
    : QWidget (parent)
    , m_state(CharaMangerModel::AddInfoState::StartState)
    , m_timer(new QTimer(this))
    , m_angle(0)
{
    m_timer->setInterval(1000 / 60);
    connect(m_timer, &QTimer::timeout, this, static_cast<void (QWidget::*)()>(&QWidget::update));
}

IrisInfoWidget::~IrisInfoWidget()
{

}

void IrisInfoWidget::updateState(CharaMangerModel::AddInfoState state)
{
    m_state = state;

    if (m_state == CharaMangerModel::AddInfoState::Processing)
        m_timer->start();
    else {
        m_timer->stop();
    }
    update();
}

void IrisInfoWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    QString backRes;

    if (m_state == CharaMangerModel::AddInfoState::StartState
            || m_state == CharaMangerModel::AddInfoState::Processing) {
        if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
            backRes = ":/icons/deepin/builtin/icons/dcc_auth_iris-light.svg";
        } else if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::DarkType) {
            backRes = ":/icons/deepin/builtin/icons/dcc_auth_iris-dark.svg";
        }
    } else if (m_state == CharaMangerModel::AddInfoState::Success) {
        backRes = ":/icons/deepin/builtin/icons/dcc_auth_success.svg";
    } else if (m_state == CharaMangerModel::AddInfoState::Fail) {
        backRes = ":/icons/deepin/builtin/icons/dcc_auth_fail.svg";
    } else {
        qWarning() << "not support";
    }

    // 静态背景图
    QPixmap backPix(backRes);
    const int minSize = qMin(width(), height());
    const qreal scale = backPix.width() * 1.0 / minSize;
    backPix = backPix.scaled(minSize, minSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter.drawPixmap(rect(), backPix);

    // 动态效果
    if (m_state == CharaMangerModel::AddInfoState::Processing) {
        // 内圈
        painter.translate(width() / 2, height() / 2);
        painter.rotate((m_angle += 1) % 360);
        painter.translate( -(width() / 2), -(height() / 2));
        QPixmap insidePix(":/icons/deepin/builtin/icons/dcc_auth_circle_inside.svg");
        insidePix = insidePix.scaled(insidePix.width() / scale, insidePix.height() / scale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QRect insideRect((width() - insidePix.width() / scale) / 2, (height() - insidePix.height() / scale) / 2, insidePix.width() / scale, insidePix.height() / scale);
        painter.drawPixmap(insideRect, insidePix);

        // 外圈
        painter.translate(width() / 2, height() / 2);
        painter.rotate((m_angle += 2) % 360);
        painter.translate( -(width() / 2), -(height() / 2));
        QPixmap outsidePix(":/icons/deepin/builtin/icons/dcc_auth_circle_outside.svg");
        outsidePix = outsidePix.scaled(outsidePix.width() / scale, outsidePix.height() / scale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QRect outsideRect((width() - outsidePix.width() / scale) / 2, (height() - outsidePix.height() / scale) / 2, outsidePix.width() / scale, outsidePix.height() / scale);
        painter.drawPixmap(outsideRect, outsidePix);
    }

    QWidget::paintEvent(event);
}

