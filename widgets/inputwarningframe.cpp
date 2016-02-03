/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "inputwarningframe.h"

InputWarningFrame::InputWarningFrame(QWidget *sourceInput) :
    DArrowRectangle(DArrowRectangle::ArrowTop, NULL),m_sourceInput(sourceInput)
{
    D_THEME_INIT_WIDGET(InputWarningFrame);

    setArrowX(ARROW_LEFT_MARGIN);
    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::ToolTip) ;
    setMaximumWidth(sourceInput->maximumWidth());
    setShadowBlurRadius(0);
    setShadowDistance(0);

    m_label = new QLabel;
    m_label->setObjectName("WarningMessageLabel");
    m_label->setWordWrap(true);

    m_frame = new QFrame;
    QHBoxLayout *frameLayout = new QHBoxLayout(m_frame);
    frameLayout->setContentsMargins(0, 0, 0, 0);
    frameLayout->addWidget(m_label);
    setContent(m_frame);
}

void InputWarningFrame::showWarning(const QString &msg)
{
    m_label->setText(msg);
    QRect contentRect = QRect(0, 0, m_sourceInput->maximumWidth() - 2 * margin(), 0);
    QFontMetrics metrics(m_label->font());
    const int msgWidth = metrics.boundingRect(contentRect, Qt::AlignLeft | Qt::TextWordWrap, msg).width();
    m_label->setFixedWidth(msgWidth);
    m_frame->adjustSize();
    resizeWithContent();
    QPoint sourcePoint = m_sourceInput->mapToGlobal(QPoint(0, 0));

    show(sourcePoint.x() + msgWidth / 2  + margin(),
         sourcePoint.y() + m_sourceInput->height());
}
