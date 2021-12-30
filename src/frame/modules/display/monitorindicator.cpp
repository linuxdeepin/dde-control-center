/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "monitorindicator.h"

#include <QPainter>
#include <QApplication>
#include <QDebug>

#define LINE_WIDTH 10

using namespace dcc;
using namespace dcc::display;

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
    m_topLine->setGeometry(topLeft.x(), topLeft.y(), width(), LINE_WIDTH);
    m_bottomLine->setGeometry(topLeft.x(), topLeft.y() + height() - LINE_WIDTH, width(), LINE_WIDTH);
    m_rightLine->setGeometry(topLeft.x() + width() - LINE_WIDTH, topLeft.y(), LINE_WIDTH, height());
    m_leftLine->setGeometry(topLeft.x(), topLeft.y(), LINE_WIDTH, height());
}
