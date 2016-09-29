/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef FRAMEPROXYINTERFACE_H
#define FRAMEPROXYINTERFACE_H

class QWidget;
class QString;

namespace dde {

class FrameProxyInterface
{
public:
    // Module请求进入第三级页面；
    virtual void pushWidget(QString moduleId, QWidget *w) = 0;
};

}

#endif // FRAMEPROXYINTERFACE_H
