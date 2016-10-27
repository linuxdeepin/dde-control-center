/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SOUNDMODULE_H
#define SOUNDMODULE_H

#include <QObject>

#include "moduleinterface.h"
#include "soundwidget.h"
#include "soundworker.h"

class SoundModule : public QObject, public ModuleInterface
{
    Q_OBJECT
public:
    explicit SoundModule(FrameProxyInterface * frame, QObject *parent = 0);

    void initialize() Q_DECL_OVERRIDE;
    const QString name() const Q_DECL_OVERRIDE;
    void moduleActive() Q_DECL_OVERRIDE;
    void moduleDeactive() Q_DECL_OVERRIDE;
    ModuleWidget *moduleWidget() Q_DECL_OVERRIDE;
    void contentPopped(ContentWidget * const w) Q_DECL_OVERRIDE;

private:
    SoundWidget *m_soundWidget;
    SoundWorker *m_soundWorker;
};

#endif // SOUNDMODULE_H
