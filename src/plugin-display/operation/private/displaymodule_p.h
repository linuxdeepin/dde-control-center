// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DISPLAYMODULE_P_H
#define DISPLAYMODULE_P_H

#include <QObject>

namespace dccV25 {
class DisplayWorker;
class DisplayModel;
class DisplayModule;
class DccScreen;

class DisplayModulePrivate
{

public:
    explicit DisplayModulePrivate(DisplayModule *parent);

    virtual ~DisplayModulePrivate() { }

    void init();
    void updateVirtualScreens();
    void updateMonitorList();
    void updatePrimary();
    void updateDisplayMode();
    DccScreen *primary() const;
    QString displayMode() const;

public:
    DisplayModule *q_ptr;
    DisplayModel *m_model;
    DisplayWorker *m_worker;
    QList<DccScreen *> m_screens;
    QList<DccScreen *> m_virtualScreens;
    DccScreen *m_primary;
    QString m_displayMode;

    Q_DECLARE_PUBLIC(DisplayModule)
};
} // namespace dccV25
#endif // DISPLAYMODULE_P_H
