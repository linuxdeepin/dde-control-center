//Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef UTILS_H
#define UTILS_H

#include <DSysInfo>
#include <DNotifySender>

const static Dtk::Core::DSysInfo::UosType UosType = Dtk::Core::DSysInfo::uosType();
const static bool IsServerSystem = (Dtk::Core::DSysInfo::UosServer == UosType); //是否是服务器版

#endif // UTILS_H
