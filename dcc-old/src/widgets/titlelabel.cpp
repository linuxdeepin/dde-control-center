//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "widgets/titlelabel.h"
#include "widgets/accessibleinterface.h"

#include <DFontSizeManager>

#include <QFont>
#include <QEvent>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;
SET_LABEL_ACCESSIBLE(TitleLabel,"TitleLabel");
TitleLabel::TitleLabel(QWidget *parent, Qt::WindowFlags f)
    : DLabel(parent, f)
{
    auto tf = this->font();
    tf.setWeight(QFont::Medium);
    setFont(tf);

    setContentsMargins(10, 0, 0, 0);
    setForegroundRole(DPalette::TextTitle);
    DFontSizeManager::instance()->bind(this,DFontSizeManager::T5, QFont::DemiBold);
}

TitleLabel::TitleLabel(const QString &text, QWidget *parent, Qt::WindowFlags f)
    : TitleLabel(parent, f)
{
    setText(text);
}

bool TitleLabel::event(QEvent *e)
{
    if (e->type() == QEvent::ApplicationFontChange) {
        auto tf = this->font();
        tf.setWeight(QFont::Medium);
        setFont(tf);
    }

    return QLabel::event(e);
}
