/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DCC_SOUND_ADVANCEDPAGE_H
#define DCC_SOUND_ADVANCEDPAGE_H

#include "contentwidget.h"
#include "settingsgroup.h"
#include "portitem.h"
#include "soundmodel.h"

namespace dcc {
namespace sound {

class AdvancedPage : public ContentWidget
{
    Q_OBJECT
public:
    explicit AdvancedPage(SoundModel *model);
    ~AdvancedPage();

    void setModel(SoundModel *model);

signals:
    void requestSetPort(const Port *port);

private slots:
    void addPort(const Port *port);
    void removePort(const QString &portId, const uint &cardId);

private:
    SoundModel *m_model;
    SettingsGroup *m_outputGroup;
    SettingsGroup *m_inputGroup;
    QList<PortItem *> m_portItemList;
};

} // namespace sound
} // namespace dcc

#endif // DCC_SOUND_ADVANCEDPAGE_H
