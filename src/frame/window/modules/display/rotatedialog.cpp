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

#include "modules/display/displaymodel.h"
#include "widgets/basiclistdelegate.h"

#include <QVBoxLayout>
#include <QMouseEvent>
#include <QLabel>
#include <QPixmap>

using namespace dcc::display;
using namespace DCC_NAMESPACE::display;

RotateDialog::RotateDialog(Monitor *mon, QWidget *parent)
    : QDialog(parent)
    , m_mon(mon)
{
    QVBoxLayout *centralLayout = new QVBoxLayout;

    setFixedSize(300, 300);

    setMouseTracking(true);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::Tool | Qt::WindowStaysOnTopHint);

    const qreal ratio = devicePixelRatioF();
    QPixmap rotatePixmap = loadPixmap(":/display/themes/common/icon/rotate.svg");
    QLabel *osd = new QLabel;
    osd->setPixmap(rotatePixmap);
    osd->setFixedSize(rotatePixmap.width() / ratio, rotatePixmap.height() / ratio);

    centralLayout->addWidget(osd, Qt::AlignHCenter);
    centralLayout->setAlignment(osd, Qt::AlignCenter);

    qApp->setOverrideCursor(Qt::BlankCursor);

    setLayout(centralLayout);
}

RotateDialog::~RotateDialog()
{
    qApp->restoreOverrideCursor();
}

void RotateDialog::setModel(dcc::display::DisplayModel *model)
{
    m_model = model;

    const qreal ratio = devicePixelRatioF();
    Monitor *mon = m_mon ? m_mon : m_model->primaryMonitor();
    setFixedWidth(mon->w() / ratio);
    setFixedHeight(mon->w() / ratio);

    move(mon->rect().topLeft());
    QCursor::setPos(rect().center() + pos());
}

void RotateDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        reject();
        return;
    }

    if (event->matches(QKeySequence::StandardKey::Save)) {
        accept();
    }
}

void RotateDialog::mouseReleaseEvent(QMouseEvent *e)
{
    e->accept();

    switch (e->button()) {
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

    if (m_mon)
        Q_EMIT RotateDialog::requestRotate(m_mon, nextValue);
    else
        Q_EMIT RotateDialog::requestRotateAll(nextValue);

//    m_resetTimeout = ResetOperationTimeLimit;
//    m_resetOperationTimer->start();
    update();
}
