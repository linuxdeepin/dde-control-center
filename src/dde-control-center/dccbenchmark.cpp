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

} // namespace dccV25
