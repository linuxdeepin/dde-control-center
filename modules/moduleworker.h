/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef MODULEWORKER_H
#define MODULEWORKER_H

#include <QObject>

class ModuleWorker
{
protected:
    virtual void activate() = 0;
    virtual void deactivate() = 0;
};

#endif // MODULEWORKER_H
