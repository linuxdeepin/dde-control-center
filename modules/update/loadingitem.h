/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef LOADINGITEM_H
#define LOADINGITEM_H

#include "settingsitem.h"

namespace dcc {
namespace widgets {
class NormalLabel;
class LoadingIndicator;
}
}

namespace dcc {
namespace update {

class LoadingItem : public dcc::widgets::SettingsItem
{
    Q_OBJECT

public:
    explicit LoadingItem(QFrame* parent = 0);

    void setIndicatorVisible(bool visible);
    void setMessage(const QString& message);

private:
    dcc::widgets::NormalLabel* m_messageLabel;
    dcc::widgets::LoadingIndicator* m_indicator;
};

} // namespace widgets
} // namespace dcc

#endif // LOADINGITEM_H
