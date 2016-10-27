/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SOUNDWORKER_H
#define SOUNDWORKER_H

#include <QObject>

#include "moduleworker.h"

class SoundWorker : public QObject, public ModuleWorker
{
    Q_OBJECT
public:
    SoundWorker(QObject * parent = 0);

    void activate() Q_DECL_OVERRIDE;
    void deactivate() Q_DECL_OVERRIDE;
};

#endif // SOUNDWORKER_H
