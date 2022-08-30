// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include "interface/namespace.h"
#include "widgets/contentwidget.h"

namespace dcc {
namespace display {
class DisplayModel;
}

} // namespace dcc

namespace DCC_NAMESPACE {

namespace display {

class DisplayWidget : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit DisplayWidget(QWidget *parent = nullptr);

public:
    void setModel();
    int showPath(const QString &path);
};

} // namespace display

} // namespace DCC_NAMESPACE

#endif // DISPLAYWIDGET_H
