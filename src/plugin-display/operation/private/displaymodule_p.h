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
class ScreenData;

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
    void updateMaxGlobalScale();
    DccScreen *primary() const;
    QString displayMode() const;
    void setScreenPosition(QList<ScreenData *> screensData);

public:
    DisplayModule *q_ptr;
    DisplayModel *m_model;
    DisplayWorker *m_worker;
    QList<DccScreen *> m_screens;
    QList<DccScreen *> m_virtualScreens;
    DccScreen *m_primary;
    QString m_displayMode;
    qreal m_maxGlobalScale;

    Q_DECLARE_PUBLIC(DisplayModule)
};
} // namespace dccV25
#endif // DISPLAYMODULE_P_H
