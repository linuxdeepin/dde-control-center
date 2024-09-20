// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick.Controls 2.15

import org.deepin.dtk 1.0 as D
import "DccUtils.js" as DccUtils

DccObject {
    pageType: DccObject.Item
    page: Label {
        font: DccUtils.copyFont(D.DTK.fontManager.t4, {
                                    "bold": true
                                })
        text: dccObj.displayName
    }
}
