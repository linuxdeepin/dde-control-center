//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "monitorindicator.h"

#include <QPainter>
#include <QApplication>
#include <QDebug>

#define LINE_WIDTH 10

using namespace DCC_NAMESPACE;

MonitorIndicator::MonitorIndicator(QWidget *parent)
    : QFrame(nullptr)
    , m_topLine(new QFrame(nullptr))
    , m_bottomLine(new QFrame(nullptr))
    , m_leftLine(new QFrame(nullptr))
    , m_rightLine(new QFrame(nullptr))
{
    Q_UNUSED(parent)
    QFrame::setVisible(false);

    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, QColor("#2ca7f8"));

    m_topLine->setWindowFlags(Qt::CoverWindow | Qt::WindowStaysOnTopHint | Qt::SplashScreen | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
    m_topLine->setAutoFillBackground(true);
    m_topLine->setPalette(pal);

    m_bottomLine->setWindowFlags(Qt::CoverWindow | Qt::WindowStaysOnTopHint | Qt::SplashScreen | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
    m_bottomLine->setAutoFillBackground(true);
    m_bottomLine->setPalette(pal);

    m_leftLine->setWindowFlags(Qt::CoverWindow | Qt::WindowStaysOnTopHint | Qt::SplashScreen | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
    m_leftLine->setAutoFillBackground(true);
    m_leftLine->setPalette(pal);

    m_rightLine->setWindowFlags(Qt::CoverWindow | Qt::WindowStaysOnTopHint | Qt::SplashScreen | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
    m_rightLine->setAutoFillBackground(true);
    m_rightLine->setPalette(pal);
}

MonitorIndicator::~MonitorIndicator()
{
    delete m_topLine;
    delete m_bottomLine;
    delete m_leftLine;
    delete m_rightLine;
}

void MonitorIndicator::setVisible(bool visible)
{
    updateGeometry();
    m_topLine->setVisible(visible);
    m_bottomLine->setVisible(visible);
    m_leftLine->setVisible(visible);
    m_rightLine->setVisible(visible);
}

void MonitorIndicator::updateGeometry()
{
    QPoint topLeft = mapToGlobal(QPoint(0,0));
    int lineWidth = static_cast<int>(LINE_WIDTH / qApp->devicePixelRatio());
    m_topLine->setGeometry(topLeft.x(), topLeft.y(), width(), lineWidth);
    m_bottomLine->setGeometry(topLeft.x(), topLeft.y() + height() - lineWidth, width(), lineWidth);
    m_rightLine->setGeometry(topLeft.x() + width() - lineWidth, topLeft.y(), lineWidth, height());
    m_leftLine->setGeometry(topLeft.x(), topLeft.y(), lineWidth, height());
}
