// SPDX-FileCopyrightText: 2015 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LOADINGNEXTPAGEWIDGET_H
#define LOADINGNEXTPAGEWIDGET_H

#include "widgets/nextpagewidget.h"

#include <dpicturesequenceview.h>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace widgets {

class LoadingNextPageWidget : public NextPageWidget
{
    Q_OBJECT

public:
    explicit LoadingNextPageWidget(QFrame *parent = nullptr);

public Q_SLOTS:
    void setLoading(const bool loading);

private:
    DPictureSequenceView *m_loadingIndicator;
};

}

}

#endif // LOADINGNEXTPAGEWIDGET_H
