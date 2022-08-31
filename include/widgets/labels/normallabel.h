// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef NORMALLABEL_H
#define NORMALLABEL_H

#include <QLabel>

namespace dcc {
namespace widgets {

class NormalLabel : public QLabel
{
    Q_OBJECT

public:
    using QLabel::QLabel; // using default inheritance constructor
};

}
}

#endif // NORMALLABEL_H
