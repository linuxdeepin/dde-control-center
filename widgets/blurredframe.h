/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef BLURREDFRAME_H
#define BLURREDFRAME_H

#include <QFrame>

namespace dcc {
namespace widgets {

class BlurredFrame : public QFrame
{
    Q_OBJECT
public:
    explicit BlurredFrame(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *event);
};

} // namespace widgets
} // namespace dcc

#endif // BLURREDFRAME_H
