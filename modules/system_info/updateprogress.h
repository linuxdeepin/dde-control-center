/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef UPDATEPROGRESS_H
#define UPDATEPROGRESS_H

#include <dcircleprogress.h>
#include <dpicturesequenceview.h>

DWIDGET_USE_NAMESPACE

class UpdateProgress : public DCircleProgress
{
public:
    explicit UpdateProgress(QWidget *parent = nullptr);

    inline void showLoading() {m_picSeq->show();}
    inline void hideLoading() {m_picSeq->hide();}

private:
    DPictureSequenceView *m_picSeq;
};

#endif // UPDATEPROGRESS_H
