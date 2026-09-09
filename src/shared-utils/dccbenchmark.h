// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QElapsedTimer>
#include <QHash>
#include <QLoggingCategory>
#include <QString>

Q_DECLARE_LOGGING_CATEGORY(dccBenchmarkLog)

namespace dccV25 {

class DccBenchmark
{
public:
    DccBenchmark(const QString &plugin, const char *stage);
    ~DccBenchmark();

    DccBenchmark(const DccBenchmark &) = delete;
    DccBenchmark &operator=(const DccBenchmark &) = delete;

private:
    QString m_plugin;
    const char *m_stage;
    QElapsedTimer m_timer;
};

class DccAppTimeline
{
public:
    static DccAppTimeline &instance();

    DccAppTimeline(const DccAppTimeline &) = delete;
    DccAppTimeline &operator=(const DccAppTimeline &) = delete;

    void start();
    void stop();
    void log(const QString &stage, const QString &detail = QString()) const;

private:
    DccAppTimeline() = default;

    QElapsedTimer m_timer;
};

class DccLoadTimer
{
public:
    DccLoadTimer() = default;

    DccLoadTimer(const DccLoadTimer &) = delete;
    DccLoadTimer &operator=(const DccLoadTimer &) = delete;

    void start();                          // Start global timer, clear per-plugin tracking
    void addPlugin(const QString &name);  // Record a plugin's start timestamp
    void finishPlugin(const QString &name);  // Calculate & log per-plugin total time
    void stop();                           // Stop timer and log all-plugins total time

private:
    QElapsedTimer m_totalTimer;
    QHash<QString, qint64> m_pluginStartTimes;
};

} // namespace dccV25

#define DCC_BENCHMARK_CONCAT_IMPL(a, b) a##b
#define DCC_BENCHMARK_CONCAT(a, b) DCC_BENCHMARK_CONCAT_IMPL(a, b)
#define DCC_BENCHMARK(plugin, stage) \
    const dccV25::DccBenchmark DCC_BENCHMARK_CONCAT(DccBenchmark_, __LINE__)(plugin, stage)
