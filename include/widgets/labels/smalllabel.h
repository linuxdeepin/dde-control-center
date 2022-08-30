// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SMALLLABEL_H
#define SMALLLABEL_H

#include <QLabel>

namespace dcc {
namespace widgets {

class SmallLabel : public QLabel
{
    Q_OBJECT

public:
    using QLabel::QLabel; // using default inheritance constructor
};

}
}

#endif // SMALLLABEL_H
