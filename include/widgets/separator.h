// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SEPARATOR_H
#define SEPARATOR_H

#include <QFrame>

namespace dcc {
namespace widgets {

class HSeparator : public QFrame
{
    Q_OBJECT
public:
    explicit HSeparator(QWidget *parent = 0);
};

class VSeparator : public QFrame
{
    Q_OBJECT
public:
    explicit VSeparator(QWidget *parent = 0);
};

} // namespace widgets
} // namespace dcc

#endif // SEPARATOR_H
