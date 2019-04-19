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

#include "rotatedialog.h"
#include "displayworker.h"
#include "monitor.h"
#include "displaymodel.h"
#include "widgets/basiclistdelegate.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QFontMetrics>
#include <QApplication>
#include <QWindow>
#include <QPainter>
#include <QPaintEvent>
#include <QVBoxLayout>
#include <QLabel>

#include <DBlurEffectWidget>

#include <com_deepin_daemon_inputdevice_mouse.h>

#define ResetOperationTimeLimit 15

DWIDGET_USE_NAMESPACE

using namespace dcc::display;
using MouseInter = com::deepin::daemon::inputdevice::Mouse;

RotateDialog::RotateDialog(Monitor *mon, QWidget *parent)
    : QDialog(parent),

      m_mon(mon),
      m_model(nullptr),
      m_adjustDelayTimer(new QTimer(this)),
      m_wmHelper(DWindowManagerHelper::instance()),
      m_mouseLeftHand(false),
      m_changed(false)
{
    const qreal ratio = devicePixelRatioF();

    DBlurEffectWidget *blurWidget = new DBlurEffectWidget;
    blurWidget->setFixedSize(140, 140);
    blurWidget->setBlurRectXRadius(10);
    blurWidget->setBlurRectYRadius(10);
    blurWidget->setMaskColor(DBlurEffectWidget::LightColor);
    blurWidget->setBlendMode(DBlurEffectWidget::BehindWindowBlend);

    QPixmap rotatePixmap = loadPixmap(":/display/themes/common/icon/rotate.svg");
    QLabel *osd = new QLabel;
    osd->setPixmap(rotatePixmap);
    osd->setFixedSize(rotatePixmap.width() / ratio, rotatePixmap.height() / ratio);

    QVBoxLayout *l = new QVBoxLayout(blurWidget);
    l->setMargin(0);
    l->setSpacing(0);
    l->addWidget(osd, Qt::AlignHCenter);
    l->setAlignment(osd, Qt::AlignCenter);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addWidget(blurWidget);
    centralLayout->setAlignment(blurWidget, Qt::AlignCenter);
    centralLayout->setMargin(0);
    centralLayout->setSpacing(0);

    connect(m_mon, &Monitor::wChanged, m_adjustDelayTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(m_mon, &Monitor::hChanged, m_adjustDelayTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(m_mon, &Monitor::xChanged, m_adjustDelayTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(m_mon, &Monitor::yChanged, m_adjustDelayTimer, static_cast<void (QTimer::*)()>(&QTimer::start));

    MouseInter *mInter = new MouseInter("com.deepin.daemon.InputDevices", "/com/deepin/daemon/InputDevice/Mouse", QDBusConnection::sessionBus(), this);
    m_mouseLeftHand = mInter->leftHanded();

    setLayout(centralLayout);

    init();
}

RotateDialog::RotateDialog(DisplayModel *model, QWidget *parent)
    : QDialog(parent),

      m_mon(nullptr),
      m_model(model),
      m_adjustDelayTimer(new QTimer(this)),
      m_wmHelper(DWindowManagerHelper::instance())
{
    connect(m_model, &DisplayModel::screenWidthChanged, m_adjustDelayTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(m_model, &DisplayModel::screenHeightChanged, m_adjustDelayTimer, static_cast<void (QTimer::*)()>(&QTimer::start));

    init();
}

void RotateDialog::init()
{
    setMouseTracking(true);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::Tool | Qt::WindowStaysOnTopHint);

    connect(m_adjustDelayTimer, &QTimer::timeout, this, &RotateDialog::adjustGemotry);

    qApp->setOverrideCursor(Qt::BlankCursor);

    m_adjustDelayTimer->setSingleShot(true);
    m_adjustDelayTimer->setInterval(100);
    m_adjustDelayTimer->start();

    m_resetOperationTimer = new QTimer(this);
    m_resetTimeout = ResetOperationTimeLimit;

    // Note: Using normal method cannot translate this string
    const char* NoopTips = QT_TRANSLATE_NOOP(
                "RotateDialog",
                "Left click to rotate, right click to restore and exit, press Ctrl+S to save.\nIf no operation, the display will be restored after %1s.");
    m_tips = QApplication::translate("RotateDialog", NoopTips);

    m_resetOperationTimer->setInterval(1000);
    connect(m_resetOperationTimer, &QTimer::timeout, this, [=] {
        m_resetTimeout--;
        update();
        if (m_resetTimeout < 1) {
            reject();
        }
    });

}

RotateDialog::~RotateDialog()
{
    qApp->restoreOverrideCursor();
}

void RotateDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        reject();
        return;
    }

    // another implementation
//    if (event->modifiers() == Qt::ControlModifier
//            && event->key() == Qt::Key_S) {
//        accept();
//    }

    if (event->matches(QKeySequence::StandardKey::Save)) {
        accept();
    }
}

void RotateDialog::mouseReleaseEvent(QMouseEvent *e)
{
    e->accept();

    switch (e->button())
    {
    case Qt::RightButton:   reject();       break;
    case Qt::LeftButton: {
        rotate();
        m_changed = true;
        break;
    }
    default:;
    }
}

void RotateDialog::mouseMoveEvent(QMouseEvent *e)
{
    QDialog::mouseMoveEvent(e);

    QCursor::setPos(rect().center() + pos());
}

void RotateDialog::leaveEvent(QEvent *e)
{
    QDialog::leaveEvent(e);

    QCursor::setPos(rect().center() + pos());
}

void RotateDialog::paintEvent(QPaintEvent *e)
{
    QDialog::paintEvent(e);

    QString tips = m_tips.arg(m_resetTimeout);
    if (!m_changed) {
        tips = tips.split("\n").first();
    }

    int margin = 100;

    QPainter painter(this);
    if (m_wmHelper->hasComposite())
    {
        painter.fillRect(rect(), QColor(0, 0, 0, 255 * .6));
        painter.setPen(Qt::white);
    }
    else
    {
        margin = 60;

        painter.fillRect(rect(), Qt::white);
        painter.setPen(Qt::black);
    }

    QRect destHRect(0, 0, width(), margin);
    QRect destVRect(0, 0, height(), margin);

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

void RotateDialog::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);

    QTimer::singleShot(1, this, &RotateDialog::grabKeyboard);
}

void RotateDialog::rotate()
{
    Monitor *mon = m_mon ? m_mon : m_model->primaryMonitor();
    Q_ASSERT(mon);

    const auto rotates = mon->rotateList();
    const auto rotate = mon->rotate();
    const int s = rotates.size();

    Q_ASSERT(rotates.contains(rotate));

    const quint16 nextValue = m_mouseLeftHand
            ? rotates[(rotates.indexOf(rotate) - 1 + s) % s]
            : rotates[(rotates.indexOf(rotate) + 1) % s];

    if (m_mon)
        Q_EMIT requestRotate(m_mon, nextValue);
    else
        Q_EMIT requestRotateAll(nextValue);

    m_resetTimeout = ResetOperationTimeLimit;
    m_resetOperationTimer->start();
    update();
}

void RotateDialog::adjustGemotry()
{
    const bool composite = m_wmHelper->hasComposite();
    const qreal ratio = devicePixelRatioF();

    if (composite)
    {
        if (m_mon)
        {
            setFixedWidth(m_mon->w() / ratio);
            setFixedHeight(m_mon->h() / ratio);
            move(m_mon->x(), m_mon->y());
        } else {
            setFixedWidth(m_model->screenWidth());
            setFixedHeight(m_model->screenHeight());
            move(0, 0);
        }
    } else {
        setFixedWidth(500);
        setFixedHeight(500);

        if (m_mon) {
            QRectF r(rect());
            r.moveCenter(m_mon->rect().center() / ratio);
            setGeometry(r.toRect());
        }
        else
            move((m_model->screenWidth() - width()) / 2, m_model->screenHeight() - height() / 2);
    }

    QCursor::setPos(geometry().center());
}
