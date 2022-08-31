// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef APPSTOREUPDATEITEM_H
#define APPSTOREUPDATEITEM_H

#include "updatesettingitem.h"

namespace dcc {
namespace update {


class AppstoreUpdateItem:  public UpdateSettingItem
{
    Q_OBJECT
public:
    explicit AppstoreUpdateItem(QWidget *parent = nullptr);

    void showMore() override;
    void init();

public: Q_SIGNALS:
    void requestOpenAppStroe();
};


}
}
#endif // APPSTOREUPDATEITEM_H
