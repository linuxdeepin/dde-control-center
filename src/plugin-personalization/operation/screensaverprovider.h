// SPDX-FileCopyrightText: 2025-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "operation/model/wallpapermodel.h"
#include "personalizationdbusproxy.h"
#include "personalizationmodel.h"

class ScreensaverWorker : public QObject
{
    Q_OBJECT
public:
    explicit ScreensaverWorker(PersonalizationDBusProxy *proxy, QObject *parent = nullptr);
    void terminate();
Q_SIGNALS:
    void pushScreensaver(const QList<WallpaperItemPtr> &items);
public Q_SLOTS:
    void list();
private:
    PersonalizationDBusProxy *m_proxy = nullptr;
    volatile bool running = false;
};

class ScreensaverProvider : public QObject
{
    Q_OBJECT
public:
    explicit ScreensaverProvider(PersonalizationDBusProxy *proxy, PersonalizationModel *model, QObject *parent = nullptr);
    ~ScreensaverProvider() override;
    void fecthData();

private Q_SLOTS:
    void setScreensaver(const QList<WallpaperItemPtr> &items);
private:
    QThread *workThread = nullptr;
    ScreensaverWorker *worker = nullptr;
    PersonalizationModel *m_model = nullptr;
    PersonalizationDBusProxy *m_proxy = nullptr;
};
