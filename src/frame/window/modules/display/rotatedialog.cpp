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

#include <QVBoxLayout>
#include <QMouseEvent>

using namespace dcc::display;
using namespace DCC_NAMESPACE::display;

RotateDialog::RotateDialog(Monitor *mon, QWidget *parent)
    : QDialog(parent)
{
    QVBoxLayout *centralLayout = new QVBoxLayout;

    setMouseTracking(true);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::Tool | Qt::WindowStaysOnTopHint);

    setLayout(centralLayout);
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
        Q_EMIT requestRotate(m_mon, nextValue);
    else
        Q_EMIT requestRotateAll(nextValue);

//    m_resetTimeout = ResetOperationTimeLimit;
//    m_resetOperationTimer->start();
//    update();

}
