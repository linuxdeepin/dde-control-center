// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dccbenchmark.h"

#include <QDebug>

Q_LOGGING_CATEGORY(dccBenchmarkLog, "dde.dcc.benchmark")

namespace dccV25 {

DccBenchmark::DccBenchmark(const QString &plugin, const char *stage)
    : m_plugin(plugin)
    , m_stage(stage)
{
    m_timer.start();
}

DccBenchmark::~DccBenchmark()
{
    const auto elapsed = m_timer.elapsed();

    qCDebug(dccBenchmarkLog).noquote()
            << QStringLiteral("\"%1\" %2 in %3 ms.")
                       .arg(m_plugin, QString::fromLatin1(m_stage))
                       .arg(elapsed);
}

DccAppTimeline &DccAppTimeline::instance()
{
    static DccAppTimeline timer;
    return timer;
}

void DccAppTimeline::start()
{
    m_timer.start();
}

void DccAppTimeline::stop()
{
    if (!m_timer.isValid())
        return;

    m_timer.invalidate();
}

void DccAppTimeline::log(const QString &stage, const QString &detail) const
{
    if (!m_timer.isValid()) {
        return;
    }

    QString message = QStringLiteral("Dcc-timeline: %1").arg(stage);
    if (!detail.isEmpty()) {
        message += QStringLiteral(" \"%1\"").arg(detail);
    }
    message += QStringLiteral(" in %1 ms.").arg(m_timer.elapsed());
    qCDebug(dccBenchmarkLog).noquote() << message;
}

void DccLoadTimer::start()
{
    qCDebug(dccBenchmarkLog).noquote() << QStringLiteral("Begin-to-load-plugins.");
    m_totalTimer.start();
    m_pluginStartTimes.clear();
}

void DccLoadTimer::addPlugin(const QString &name)
{
    m_pluginStartTimes.insert(name, m_totalTimer.elapsed());
}

void DccLoadTimer::finishPlugin(const QString &name)
{
    if (m_pluginStartTimes.contains(name)) {
        const qint64 startTime = m_pluginStartTimes.value(name);
        qCDebug(dccBenchmarkLog).noquote()
                << QStringLiteral("\"%1\" total-loading-finished in %2 ms.")
                           .arg(name)
                           .arg(m_totalTimer.elapsed() - startTime);
    }
}

void DccLoadTimer::stop()
{
    if (!m_totalTimer.isValid())
        return;

    qCDebug(dccBenchmarkLog).noquote()
            << QStringLiteral("All %1 plugins-loaded in %2 ms.")
                       .arg(m_pluginStartTimes.size())
                       .arg(m_totalTimer.elapsed());
    m_totalTimer.invalidate();
    m_pluginStartTimes.clear();
}

} // namespace dccV25
