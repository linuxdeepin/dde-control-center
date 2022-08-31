// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "widgets/passwordedit.h"

#include <QKeyEvent>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace widgets {

PasswordEdit::PasswordEdit(QWidget *parent)
    : DPasswordEdit(parent)
{
    lineEdit()->setAcceptDrops(false);
    lineEdit()->setContextMenuPolicy(Qt::NoContextMenu);
    lineEdit()->installEventFilter(this);
}

bool PasswordEdit::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == lineEdit() && event->type() == QEvent::KeyPress) {
        QKeyEvent *e = dynamic_cast<QKeyEvent *>(event);
        if (e && (e->matches(QKeySequence::Copy) || e->matches(QKeySequence::Cut) || e->matches(QKeySequence::Paste))) {
            return true;
        }
    }
    if (obj == lineEdit() && event->type() == QEvent::FocusOut) {
        Q_EMIT getNewPassWdLevel(lineEdit()->text());
    }
    return DPasswordEdit::eventFilter(obj, event);
}

}
} // namespace dcc
