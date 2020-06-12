/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
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

#include "rotatedialog.h"
#include "window/utils.h"

#include "modules/display/displaymodel.h"
#include "widgets/basiclistdelegate.h"

#include <DBlurEffectWidget>

#include <QVBoxLayout>
#include <QMouseEvent>
#include <QLabel>
#include <QPixmap>
#include <QPainter>

using namespace dcc::display;
using namespace DCC_NAMESPACE::display;
using namespace Dtk::Widget;

RotateDialog::RotateDialog(Monitor *mon, QWidget *parent)
    : QDialog(parent)
    , m_mon(mon)
{
    setMouseTracking(true);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::Tool | Qt::WindowStaysOnTopHint);

    DBlurEffectWidget *blurWidget = new DBlurEffectWidget;
    blurWidget->setFixedSize(140, 140);
    blurWidget->setBlurRectXRadius(10);
    blurWidget->setBlurRectYRadius(10);
    blurWidget->setMaskColor(DBlurEffectWidget::LightColor);
    blurWidget->setBlendMode(DBlurEffectWidget::BehindWindowBlend);

    QPixmap rotatePixmap = loadPixmap(":/display/themes/common/icon/rotate.svg");
    QLabel *osd = new QLabel;
    const qreal ratio = devicePixelRatioF();
    osd->setPixmap(rotatePixmap);
    osd->setFixedSize(int(rotatePixmap.width() / ratio),
                      int(rotatePixmap.height() / ratio));

    QVBoxLayout *l = new QVBoxLayout(blurWidget);
    l->setMargin(0);
    l->setSpacing(0);
    l->addWidget(osd, Qt::AlignHCenter);
    l->setAlignment(osd, Qt::AlignCenter);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->setContentsMargins(ThirdPageContentsMargins);
    centralLayout->addWidget(blurWidget, Qt::AlignHCenter);
    centralLayout->setAlignment(blurWidget, Qt::AlignCenter);
    setLayout(centralLayout);

    m_resetOperationTimer = new QTimer(this);
    m_resetOperationTimer->setInterval(1000);
    connect(m_resetOperationTimer, &QTimer::timeout, this, [=] {
        m_resetTimeout--;
        update();
        if (m_resetTimeout < 1) {
            m_resetTimeout = 15;
            reject();
        }
    });
    m_resetOperationTimer->start();
}

RotateDialog::~RotateDialog()
{
}

void RotateDialog::setModel(dcc::display::DisplayModel *model)
{
    m_model = model;

    Monitor *mon = m_mon ? m_mon : m_model->primaryMonitor();
    connect(mon, &Monitor::wChanged, this, &RotateDialog::resetGeometry);
    connect(mon, &Monitor::hChanged, this, &RotateDialog::resetGeometry);
    connect(mon, &Monitor::xChanged, this, &RotateDialog::resetGeometry);
    connect(mon, &Monitor::yChanged, this, &RotateDialog::resetGeometry);

    resetGeometry();
}

void RotateDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        reject();
        return;
    }
    if (event->matches(QKeySequence::StandardKey::Save)) {
        qDebug() << "QKeySequence::StandardKey::Save accept()";
        accept();
    }
}

void RotateDialog::mouseReleaseEvent(QMouseEvent *e)
{
    e->accept();

    switch (e->button()) {
    case Qt::RightButton:
        reject();
        break;
    case Qt::LeftButton:
        rotate();
        m_changed = true;
        break;
    default:
        break;
    }
}

void RotateDialog::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);

    QTimer::singleShot(100, this, static_cast<void (RotateDialog::*)()>(&RotateDialog::grabMouse));
    QTimer::singleShot(1, this, &RotateDialog::grabKeyboard);
}

void RotateDialog::paintEvent(QPaintEvent *e)
{
    QDialog::paintEvent(e);

    QPainter painter(this);
    int margin = 100;
    if (m_wmHelper->hasComposite()) {
        painter.fillRect(rect(), QColor(0, 0, 0, int(255 * 0.6)));
        painter.setPen(Qt::white);
    } else {
        margin = 60;

        painter.fillRect(rect(), Qt::white);
        painter.setPen(Qt::black);
    }

    QRect destHRect(0, 0, width(), margin);
    QRect destVRect(0, 0, height(), margin);

    QString tips(tr("Left click to rotate, right click to restore and exit, press Ctrl+S to save."));
    if(m_model && m_model->mouseLeftHand())
        tips = tr("Right click to rotate, left click to restore and exit, press Ctrl+S to save.");

    tips += "\n";
    tips += tr("Save the display settings? If no operation occurs, the display will be restored in %1s.").arg(m_resetTimeout);
    // bottom
    painter.translate(0, height() - margin);
    painter.drawText(destHRect, Qt::AlignHCenter, tips);
    painter.resetTransform();

    // left
    painter.rotate(90);
    painter.translate(0, -margin);
    painter.drawText(destVRect, Qt::AlignHCenter, tips);
    painter.resetTransform();

    // top
    painter.rotate(180);
    painter.translate(-width(), -margin);
    painter.drawText(destHRect, Qt::AlignHCenter, tips);
    painter.resetTransform();

    // right
    painter.rotate(270);
    painter.translate(-height(), width() - margin);
    painter.drawText(destVRect, Qt::AlignHCenter, tips);
    painter.resetTransform();
}

void RotateDialog::mouseMoveEvent(QMouseEvent *e)
{
    QDialog::mouseMoveEvent(e);

    //当旋转过程中可能因未知原因，grabmouse会失效，
    //mouseMove消息中固定鼠标位置确保可以继续操作
    QCursor::setPos(rect().center() + pos());
}

void RotateDialog::leaveEvent(QEvent *e)
{
    QDialog::leaveEvent(e);

    QCursor::setPos(rect().center() + pos());
}

void RotateDialog::rotate()
{
    Monitor *mon = m_mon ? m_mon : m_model->primaryMonitor();
    Q_ASSERT(mon);

    const auto rotates = mon->rotateList();
    const auto rotate = mon->rotate();
    const int s = rotates.size();

    Q_ASSERT(rotates.contains(rotate));

    const quint16 nextValue = m_model->mouseLeftHand()
                              ? rotates[(rotates.indexOf(rotate) - 1 + s) % s]
                              : rotates[(rotates.indexOf(rotate) + 1) % s];

    if (m_mon) {
        qDebug() << "requestRotate(m_mon," << nextValue << ")";
        Q_EMIT RotateDialog::requestRotate(m_mon, nextValue);
    } else {
        qDebug() << "requestRotate(" << nextValue << ")";
        Q_EMIT RotateDialog::requestRotateAll(nextValue);
    }

    m_resetOperationTimer->stop();
    m_resetTimeout = 15;
    m_resetOperationTimer->start();

    QCursor::setPos(this->rect().center());
    update();
}

void RotateDialog::resetGeometry()
{
    QTimer::singleShot(100, this, [this]{
        const qreal ratio = devicePixelRatioF();
        Monitor *mon = m_mon ? m_mon : m_model->primaryMonitor();
        if (m_wmHelper->hasComposite()) {
            setFixedSize(int(mon->w() / ratio), int(mon->h() / ratio));
            move(mon->x(), mon->y());
        } else {
            setFixedSize(700, 600);
            move(((mon->w() - width()) / 2 + mon->x()) / ratio,
                 ((mon->h() - height()) / 2 + mon->y()) / ratio);
        }
    });
}
