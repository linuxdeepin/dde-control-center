// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef TIPSITEM_H
#define TIPSITEM_H

#include "widgets/settingsitem.h"

namespace dcc {

namespace widgets {

class NormalLabel;
class TipsItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit TipsItem(QWidget *parent = 0);

    void setText(const QString &txt);

private:
    NormalLabel *m_tipsLabel;
};

}

}

#endif // TIPSITEM_H
