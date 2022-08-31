// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LARGELABEL_H
#define LARGELABEL_H

#include <QLabel>

namespace dcc {
namespace widgets {

class LargeLabel : public QLabel
{
    Q_OBJECT

public:
    using QLabel::QLabel; // using default inheritance constructor
};

}
}

#endif // LARGELABEL_H
