// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef OPTIONLISTPAGE_H
#define OPTIONLISTPAGE_H

#include "widgets/contentwidget.h"

namespace dcc {

namespace widgets {

class OptionListPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit OptionListPage(const QStringList &options, QWidget *parent = 0);

Q_SIGNALS:

private:
    const QStringList m_options;
};

}

}

#endif // OPTIONLISTPAGE_H
