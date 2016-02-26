/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "updateprogress.h"

DUI_USE_NAMESPACE

UpdateProgress::UpdateProgress(QWidget *parent) :
    DCircleProgress(parent)
{
    QStringList seq;
    for (int i(0); i != 61; ++i)
        seq.append(QString(":/images/loading/images/eLoading/eLoading%1.png").arg(i, 2, 10, QChar('0')));
    for (int i(0); i != 30; ++i)
        seq.append(QString());

    m_picSeq = new DPictureSequenceView(this);
    m_picSeq->setPictureSequence(seq);
    m_picSeq->setFixedSize(35, 35);
    m_picSeq->move(-2, -2);

    this->setProperty("backgroundColor", QColor(255, 255, 255, 200));
}

