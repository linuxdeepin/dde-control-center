//SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "operation/personalizationworker.h"

class X11Worker : public PersonalizationWorker
{
    Q_OBJECT
public:
    X11Worker(PersonalizationModel *model, QObject *parent = nullptr);
};
