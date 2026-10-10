// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef AUDIOPORTLIST_H
#define AUDIOPORTLIST_H

#include <QDBusMetaType>
#include <QList>

#include "audioport.h"

typedef QList<AudioPort> AudioPortList;

Q_DECLARE_METATYPE(AudioPortList)

void registerAudioPortListMetaType();

#endif // AUDIOPORTLIST_H
