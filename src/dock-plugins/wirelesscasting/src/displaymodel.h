// SPDX-FileCopyrightText: 2020 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DISPLAYMODEL_H
#define DISPLAYMODEL_H

#include "display1interface.h"

#include <QObject>

using DisplayInter = org::deepin::dde::Display1;

class DisplayModel : public QObject
{
    Q_OBJECT

public:
    explicit DisplayModel(QObject *parent = nullptr);
    ~DisplayModel() override;

    uchar displayMode() { return m_displayMode; }
    void setDisplayMode(const uchar &mode);
    const QStringList &screens() { return m_monitors; }
    void setMonitors(const QList<QDBusObjectPath> &monitors);
    QString primaryScreen() { return m_primaryScreen; }
    void setPrimaryScreen(const QString &screen);
    void switchMode(uchar displayMode, QString screen);
    const QString &currentMode() { return m_currentMode; }
    void setCurrentMode(const QString &currentMode);

Q_SIGNALS:
    void displayModeChanged(const uchar &mode);
    void monitorsChanged(const QStringList &monitors);
    void screenAdded(const QString &screen);
    void screenRemoved(const QString &screen);
    void primaryScreenChanged(const QString &screen);
    void currentModeChanged(const QString &mode);

protected:

private:
    DisplayInter *m_displayInter;
    uchar m_displayMode;
    QString m_primaryScreen;
    QStringList m_monitors;
    QString m_currentMode;
};
#endif // DISPLAYMODEL_H