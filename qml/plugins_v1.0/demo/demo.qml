// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import org.deepin.dcc 1.0

DccObject {
    name: "account1"
    parentName: "root"
    displayName: qsTr("account")
    icon: "accounts"
    weight: 11

    DccObject {
        name: "account1"
        parentName: "account1"
        displayName: qsTr("账户信息")
        icon: "accounts"
        weight: 11
    }
    DccObject {
        name: "uodID1"
        parentName: "account1"
        displayName: qsTr("UOS ID")
        icon: "accounts"
        weight: 21
    }
    DccObject {
        name: "securityKey1"
        parentName: "account1"
        displayName: qsTr("安全密钥")
        icon: "accounts"
        weight: 31
    }
    DccObject {
        name: "authentication1"
        parentName: "account1"
        displayName: qsTr("生物认证")
        icon: "accounts"
        weight: 41
    }
    // 系统
    DccObject {
        name: "display1"
        parentName: "system"
        displayName: qsTr("显示")
        icon: "system"
        weight: 11
    }
    DccObject {
        name: "notification1"
        parentName: "system"
        displayName: qsTr("通知")
        icon: "system"
        weight: 31
    }
    // 电源管理
    DccObject {
        name: "power1"
        parentName: "root"
        displayName: qsTr("power")
        icon: "power"
        weight: 30
    }
    DccObject {
        name: "common1"
        parentName: "power1"
        displayName: qsTr("通用")
        icon: "power"
        weight: 11
    }
    DccObject {
        name: "power1"
        parentName: "power1"
        displayName: qsTr("电源")
        icon: "power"
        weight: 21
    }
    DccObject {
        name: "battery1"
        parentName: "power1"
        displayName: qsTr("电池")
        icon: "power"
        weight: 31
    }
    // 网络
    DccObject {
        name: "network1"
        parentName: "root"
        displayName: qsTr("network")
        icon: "network"
        weight: 41
    }
    DccObject {
        name: "wireless1"
        parentName: "network1"
        displayName: qsTr("无线网络")
        icon: "network"
        weight: 11
    }
    DccObject {
        name: "vpn1"
        parentName: "network1"
        displayName: qsTr("VPN")
        icon: "network"
        weight: 21
    }
    DccObject {
        name: "systemAgent1"
        parentName: "network1"
        displayName: qsTr("系统代理")
        icon: "network"
        weight: 31
    }
    DccObject {
        name: "details1"
        parentName: "network1"
        displayName: qsTr("网络详情")
        icon: "network"
        weight: 41
    }
    // 设备
    DccObject {
        name: "bluetooth1"
        parentName: "device"
        displayName: qsTr("蓝牙")
        icon: "hardware"
        weight: 11
    }
    DccObject {
        name: "print1"
        parentName: "device"
        displayName: qsTr("打印")
        icon: "hardware"
        weight: 21
    }
    DccObject {
        name: "keyboard1"
        parentName: "device"
        displayName: qsTr("键盘")
        icon: "hardware"
        weight: 41
    }
    // 个性化
    DccObject {
        name: "personalization1"
        parentName: "root"
        displayName: qsTr("personalization")
        icon: "personalization"
        weight: 61
    }
    DccObject {
        name: "theme1"
        parentName: "personalization1"
        displayName: qsTr("主题")
        icon: "personalization"
        weight: 11
    }
    DccObject {
        name: "wallpaper1"
        parentName: "personalization1"
        displayName: qsTr("壁纸")
        icon: "personalization"
        weight: 21
    }
    DccObject {
        name: "screensaver1"
        parentName: "personalization1"
        displayName: qsTr("屏保")
        icon: "personalization"
        weight: 31
    }
    DccObject {
        name: "font1"
        parentName: "personalization1"
        displayName: qsTr("字体和字号")
        icon: "personalization"
        weight: 51
    }
    // 辅助功能
    DccObject {
        name: "auxiliary1"
        parentName: "root"
        displayName: qsTr("辅助功能")
        icon: "accessory"
        weight: 81
    }
    DccObject {
        name: "auxiliary11"
        parentName: "auxiliary1"
        displayName: qsTr("智能助手")
        icon: "accessory"
        weight: 11
    }
    DccObject {
        name: "auxiliary12"
        parentName: "auxiliary1"
        displayName: qsTr("语音听写")
        icon: "accessory"
        weight: 21
    }
    DccObject {
        name: "auxiliary13"
        parentName: "auxiliary1"
        displayName: qsTr("语音朗读")
        icon: "accessory"
        weight: 31
    }
    DccObject {
        name: "auxiliary14"
        parentName: "auxiliary1"
        displayName: qsTr("文本翻译")
        icon: "accessory"
        weight: 41
    }
    // 隐私与安全
    DccObject {
        name: "privacy1"
        parentName: "root"
        displayName: qsTr("隐私与安全")
        icon: "privacy"
        weight: 91
    }
    DccObject {
        name: "privacy11"
        parentName: "privacy1"
        displayName: qsTr("摄像头")
        icon: "privacy"
        weight: 11
    }
    DccObject {
        name: "privacy12"
        parentName: "privacy1"
        displayName: qsTr("文件管理")
        icon: "privacy"
        weight: 21
    }
    // 系统更新
    DccObject {
        name: "privacy1"
        parentName: "root"
        displayName: qsTr("系统更新")
        description: qsTr("系统更新升级")
        icon: "update"
        badge: 1
        weight: 101
    }
}
