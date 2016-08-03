/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef CLOCKITEM_H
#define CLOCKITEM_H

#include "settingsitem.h"

#include <QDateTime>

namespace dcc {
namespace widgets {
class NormalLabel;
}
}

namespace dcc {
namespace datetime {

class Clock;

class ClockItem : public dcc::widgets::SettingsItem
{
    Q_OBJECT
public:
    explicit ClockItem(QWidget *parent = 0);

private slots:
    void updateDateTime();

private:
    Clock *m_clock;
    dcc::widgets::NormalLabel *m_label;
};

} // namespace datetime
} // namespace dcc

#endif // CLOCKITEM_H
