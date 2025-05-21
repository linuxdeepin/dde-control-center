// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

import org.deepin.dcc 1.0

DccObject {
    DeepinIDLogin {
        visible: !dccData.model.loginState
    }

    DeepinIDUserInfo {
        visible: dccData.model.loginState
    }
}
