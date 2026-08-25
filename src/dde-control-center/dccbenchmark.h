// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QElapsedTimer>
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

} // namespace dccV25

#define DCC_BENCHMARK_CONCAT_IMPL(a, b) a##b
#define DCC_BENCHMARK_CONCAT(a, b) DCC_BENCHMARK_CONCAT_IMPL(a, b)
#define DCC_BENCHMARK(plugin, stage) \
    const dccV25::DccBenchmark DCC_BENCHMARK_CONCAT(DccBenchmark_, __LINE__)(plugin, stage)
