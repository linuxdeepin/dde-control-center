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
    void active() override;

public Q_SLOTS:
    void setTitleBarHeight(int value) override;
    void setWindowEffect(int value) override;
    void setMovedWindowOpacity(bool value) override;
    void setMiniEffect(int effect) override;
    void setBackgroundForMonitor(const QString &screenName, const QString &url, bool isDark) override;
    void setLockBackForMonitor(const QString &screenName, const QString &url, bool isDark) override;

private Q_SLOTS:
    void onMiniEffectChanged(bool value);

private:
    void onKWinConfigChanged(const QString &key);

private:
    Dtk::Core::DConfig *m_kwinTitleBarConfig = nullptr;
    Dtk::Core::DConfig *m_kwinCompositingConfig = nullptr;
};
