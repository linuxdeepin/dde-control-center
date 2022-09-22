// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef __DPASSWORDEDITEX__H__
#define __DPASSWORDEDITEX__H__


#include <widgets/lineeditwidget.h>

#include <DPasswordEdit>

using namespace dcc::widgets;
DWIDGET_USE_NAMESPACE

class PasswdLineEditWidget : public LineEditWidget
{
    Q_OBJECT
public:
    explicit PasswdLineEditWidget(QWidget *parent = nullptr);

    void setText(const QString &text);
};

#endif  //!__DPASSWORDEDITEX__H__
