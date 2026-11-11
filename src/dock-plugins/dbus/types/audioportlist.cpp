// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "audioportlist.h"

void registerAudioPortListMetaType()
{
    qRegisterMetaType<AudioPortList>("AudioPortList");
    qDBusRegisterMetaType<AudioPortList>();
}
