/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DCC_TRANSLUCENTFRAME_H
#define DCC_TRANSLUCENTFRAME_H

#include <QFrame>

namespace dcc {
namespace widgets {

class TranslucentFrame : public QFrame
{
    Q_OBJECT
public:
    explicit TranslucentFrame(QWidget *parent = 0);
};

}
} // namespace dcc

#endif // DCC_TRANSLUCENTFRAME_H
