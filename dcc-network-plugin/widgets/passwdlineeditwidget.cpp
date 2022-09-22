// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "passwdlineeditwidget.h"

PasswdLineEditWidget::PasswdLineEditWidget(QWidget *parent)
    : LineEditWidget(true, parent)
{
    connect(textEdit(), &QLineEdit::textChanged, this, [this] (const QString &text) {
        if (text.isEmpty())
            static_cast<DPasswordEdit*>(dTextEdit())->setEchoButtonIsVisible(true);
    });
}

void PasswdLineEditWidget::setText(const QString &text)
{
    static_cast<DPasswordEdit*>(dTextEdit())->setEchoButtonIsVisible(text.isEmpty());
    LineEditWidget::setText(text);
}
