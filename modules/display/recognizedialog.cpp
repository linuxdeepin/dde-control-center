/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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

#include "recognizedialog.h"
#include "displaymodel.h"

#include <QTimer>
#include <QPainter>

using namespace dcc::display;

const int AUTOHIDE_DELAY = 1000 * 5;

RecognizeDialog::RecognizeDialog(DisplayModel *model, QWidget *parent)
    : QDialog(parent),

      m_model(model)
{
    connect(m_model, &DisplayModel::screenHeightChanged, this, &RecognizeDialog::onScreenRectChanged);
    connect(m_model, &DisplayModel::screenWidthChanged, this, &RecognizeDialog::onScreenRectChanged);

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    onScreenRectChanged();

    // set auto hide timer
    QTimer *autoHideTimer = new QTimer(this);
    autoHideTimer->setInterval(AUTOHIDE_DELAY);
    connect(autoHideTimer, &QTimer::timeout, this, &RecognizeDialog::accept);
    autoHideTimer->start();
}

void RecognizeDialog::paintEvent(QPaintEvent *)
{
    QPen p;
    p.setWidth(1);
    p.setColor(Qt::black);

    QBrush b;
    b.setColor(Qt::white);
    b.setStyle(Qt::SolidPattern);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(p);
    painter.setBrush(b);

    if (m_model->monitorsIsIntersect())
    {
        const QRect intersectRect = QRect(0, 0, m_model->screenWidth(), m_model->screenHeight());
        QString intersectName = m_model->monitorList().first()->name();
        for (int i(1); i != m_model->monitorList().size(); ++i)
            intersectName += "=" + m_model->monitorList()[i]->name();

        paintMonitorMark(painter, intersectRect, intersectName);
    } else {
        for (auto mon : m_model->monitorList())
            paintMonitorMark(painter, mon->rect(), mon->name());
    }
}

void RecognizeDialog::onScreenRectChanged()
{
    setFixedWidth(m_model->screenWidth());
    setFixedHeight(m_model->screenHeight());
    move(0, 0);

    update();
}

void RecognizeDialog::paintMonitorMark(QPainter &painter, const QRect &rect, const QString &name)
{
    const int fontSize = rect.height() / 5;
    QFont font;
    font.setPixelSize(fontSize);
    const QFontMetrics fm(font);
    const int x = rect.center().x() - fm.width(name) / 2;
    const int y = rect.center().y() + fm.height() / 4;

    QPainterPath path;
    path.addText(x, y, font, name);
    painter.drawPath(path);
}
