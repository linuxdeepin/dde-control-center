// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"

#include <types/zoneinfo.h>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QPushButton;
QT_END_NAMESPACE

namespace dcc {
namespace datetime {
class TimezoneItem;
}
}

namespace DCC_NAMESPACE {
namespace datetime {

class SystemTimezone : public QWidget
{
    Q_OBJECT
public:
    explicit SystemTimezone(QWidget *parent = nullptr);

Q_SIGNALS:
    void requestSetSystemTimezone();

public Q_SLOTS:
    void setSystemTimezone(const ZoneInfo &zone);

private:
    QVBoxLayout *m_layout;
    dcc::datetime::TimezoneItem *m_systemTimezone;
    QPushButton *m_setSystemTimezone;
};

}// namespace datetime
}// namespace DCC_NAMESPACE
