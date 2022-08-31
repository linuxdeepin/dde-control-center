// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
