// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.0
import QtQuick.Controls 2.0
import Qt.labs.qmlmodels 1.2
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0 as D

import org.deepin.dcc 1.0

DccObject {

    DccTitleObject {
        visible: false
        name: "updateTypeGrpTitle"
        parentName: "updateSettingsPage"
        displayName: qsTr("update type")
        weight: 10
    }

    DccObject {
        visible: false
        name: "updateTypeGrp"
        parentName: "updateSettingsPage"
        weight: 20
       // visible: dccData.model().outPutPortCombo.length !== 0
        pageType: DccObject.Item
        page: DccGroupView {
            height: implicitHeight + 10
            spacing: 0

        }
        DccObject {
            name: "functionUpdate"
            parentName: "updateTypeGrp"
            displayName: qsTr("Function update")
            description: qsTr("Provide new features, quality patches, security vulnerability fixes, and fully updated content")
            weight: 10
            pageType: DccObject.Editor
            page: Switch {
               // checked: dccData.model().audioMono
                onCheckedChanged: {
                  //  dccData.worker().setAudioMono(checked)
                }
            }

        }
        DccObject {
            name: "qualityUpdate"
            parentName: "updateTypeGrp"
            displayName: qsTr("Quality Update")
            description: qsTr("Provide system quality patches")
            weight: 20
            pageType: DccObject.Editor
            page: Switch {
                // checked: dccData.model().audioMono
                onCheckedChanged: {
                    //  dccData.worker().setAudioMono(checked)
                }
            }

        }
        DccObject {
            name: "safeUpdate"
            parentName: "updateTypeGrp"
           // displayName: qsTr("安全更新")
           // description: qsTr("提供安全漏洞修复")
            weight: 30
            pageType: DccObject.Editor
            page: Switch {
                // checked: dccData.model().audioMono
                onCheckedChanged: {
                    //  dccData.worker().setAudioMono(checked)
                }
            }

        }
    }

    DccTitleObject {
        visible: false
        name: "otherSettingGrpTitle"
        parentName: "updateSettingsPage"
       // displayName: qsTr("其他设置")
        weight: 30
    }

    DccObject {
        visible: false
        name: "otherSettingGrp"
        parentName: "updateSettingsPage"
        weight: 40
        // visible: dccData.model().outPutPortCombo.length !== 0
        pageType: DccObject.Item
        page: DccGroupView {
            height: implicitHeight + 10
            spacing: 0

        }
        DccObject {
            name: "downloadLimit"
            parentName: "otherSettingGrp"
           // displayName: qsTr("下载限速")
          //  description: qsTr("提供新功能、质量补丁、安全漏洞修复登全量更新内容")
            weight: 10
            pageType: DccObject.Editor
            page: D.Switch {
                // checked: dccData.model().audioMono
                onCheckedChanged: {
                    //  dccData.worker().setAudioMono(checked)
                }
            }

        }
        DccObject {
            name: "limitSetting"
            parentName: "otherSettingGrp"
          //  displayName: qsTr("限速设置")
           // description: qsTr("提供系统质量补丁")
            weight: 20
            pageType: DccObject.Editor
            page: RowLayout {
                D.LineEdit {
                    width: Math.max(implicitWidth, editInputMinWidth)
                    text: "1024"
                    font.pixelSize: 14
                    // alertText: qsTr("长度大于等于3")
                    // showAlert: text.length >= 3
                }

                D.Label {
                    text: "KB/S"
                    font.pixelSize: 14
                }
            }

        }
    }

    DccObject {
        visible: false
        name: "autoDownloadGrp"
        parentName: "updateSettingsPage"
        weight: 50
        // visible: dccData.model().outPutPortCombo.length !== 0
        pageType: DccObject.Item
        page: DccGroupView {
            height: implicitHeight + 10
            spacing: 0

        }
        DccObject {
            name: "autoDownload"
            parentName: "autoDownloadGrp"
          //  displayName: qsTr("自动下载")
          //  description: qsTr("开启“自动下载”，会在联网的情况下自动下载更新包")
            weight: 10
            pageType: DccObject.Editor
            page: D.Switch {
                // checked: dccData.model().audioMono
                onCheckedChanged: {
                    //  dccData.worker().setAudioMono(checked)
                }
            }

        }
        DccObject {
            name: "limitSetting"
            parentName: "autoDownloadGrp"
           // displayName: qsTr("限速设置")
           // description: qsTr("提供系统质量补丁")
            weight: 20
            pageType: DccObject.Item
            page: RowLayout {
                D.CheckBox {
                    Layout.leftMargin: 14
                  //  text: qsTr("闲时下载")
                }

                RowLayout {
                    spacing: 10
                    Layout.topMargin: 5
                    Layout.bottomMargin: 5
                    Layout.rightMargin: 10

                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    D.Label {
                   //     text: qsTr("开始时间")
                    }

                    D.SpinBox {
                        value: 65
                        from: 0
                        to: 1440
                        implicitWidth: 80
                      //  implicitHeight: 30
                        font.pixelSize: 14
                        textFromValue: function (value, locale) {
                            var time = Math.floor(value / 60)
                            var timeStr = time < 10 ? ("0" + time.toString()) : time.toString()
                            var minute =  value % 60
                            var minuteStr = minute < 10 ? ("0" + minute.toString()) : minute.toString()
                            return timeStr + ":"+ minuteStr
                        }
                    }

                    D.Label {
                        Layout.leftMargin: 10
                     //   text: qsTr("结束时间")
                    }

                    D.SpinBox {
                        width: 80
                        value: 1152
                        implicitWidth: 80
                       // implicitHeight: 30
                        from: 0
                        to: 1439
                        font.pixelSize: 14
                        textFromValue: function (value, locale) {
                            var time = Math.floor(value / 60)
                            var timeStr = time < 10 ? ("0" + time.toString()) : time.toString()
                            var minute =  value % 60
                            var minuteStr = minute < 10 ? ("0" + minute.toString()) : minute.toString()
                            return timeStr + ":"+ minuteStr
                        }
                    }
                }
            }

        }
    }


    DccTitleObject {
        visible: false
        name: "advancedSettingTitle"
        parentName: "updateSettingsPage"
     //   displayName: qsTr("高级设置")
        weight: 60
    }

    DccObject {
        visible: false
        name: "advancedSettingGrp"
        parentName: "updateSettingsPage"
        weight: 70
        // visible: dccData.model().outPutPortCombo.length !== 0
        pageType: DccObject.Item
        page: DccGroupView {
            height: implicitHeight + 10
            spacing: 0

        }
        DccObject {
            name: "updateReminder"
            parentName: "advancedSettingGrp"
         //   displayName: qsTr("更新提醒")
          //  description: qsTr("提供新功能、质量补丁、安全漏洞修复登全量更新内容")
            weight: 10
            pageType: DccObject.Editor
            page: D.Switch {
                // checked: dccData.model().audioMono
                onCheckedChanged: {
                    //  dccData.worker().setAudioMono(checked)
                }
            }

        }
        DccObject {
            name: "cleanCache"
            parentName: "advancedSettingGrp"
         //   displayName: qsTr("清除软件包缓存")
           // description: qsTr("提供系统质量补丁")
            weight: 20
            pageType: DccObject.Editor
            page: D.Switch {
                // checked: dccData.model().audioMono
                onCheckedChanged: {
                    //  dccData.worker().setAudioMono(checked)
                }
            }

        }
        DccObject {
            name: "cacheTransfer"
            parentName: "advancedSettingGrp"
          //  displayName: qsTr("缓存传递")
          //  description: qsTr("启动此项，将传递更新内容至本地连接网络的电脑，以加速其他电脑下载更新")
            weight: 30
            pageType: DccObject.Editor
            page: D.Switch {
                // checked: dccData.model().audioMono
                onCheckedChanged: {
                    //  dccData.worker().setAudioMono(checked)
                }
            }

        }

        DccObject {
            name: "updateHistory"
            parentName: "advancedSettingGrp"
          //  displayName: qsTr("历史更新内容")
            //description: qsTr("启动此项，将传递更新内容至本地连接网络的电脑，以加速其他电脑下载更新")
            weight: 40
            pageType: DccObject.Editor
            page: D.Button {
            //    text: qsTr("查 看")
                onClicked: {

                }
            }
        }
    }

    DccObject {
        name: "mirrorSettingGrp"
        parentName: "updateSettingsPage"
        weight: 80
       // visible: dccData.model().outPutPortCombo.length !== 0
        pageType: DccObject.Item
        page: DccGroupView {
            height: implicitHeight + 10
            spacing: 0

        }
        DccObject {
            name: "autoMirror"
            parentName: "mirrorSettingGrp"
            displayName: qsTr("Smart Mirror Switch")
            //description: qsTr("提供新功能、质量补丁、安全漏洞修复登全量更新内容")
            weight: 10
            pageType: DccObject.Editor
            page: Switch {
                checked: dccData.model().smartMirrorSwitch
                onCheckedChanged: {
                    dccData.work().setSmartMirror(checked)
                }
            }

        }
        DccObject {
            visible: false
            name: "defautMirror"
            parentName: "mirrorSettingGrp"
         //   displayName: qsTr("默认镜像源")
            weight: 20
            pageType: DccObject.Editor
            page: D.ComboBox {
                model:["[SG]Ox.sg", "[AU]JAARNET", "[SE]Academic Computer Club", "[CN]阿里云"]
                // checked: dccData.model().audioMono

                currentIndex: 0
            }

        }
    }

    DccObject {
        visible: false
        name: "testingChannel"
        parentName: "updateSettingsPage"
     //   displayName: qsTr("内测通道")
     //   description: qsTr("加入deepin内测通道，以获取deepin最新更新内容")
        backgroundType: DccObject.Normal
        weight: 90
        pageType: DccObject.Editor
        page: D.Switch {
            // checked: dccData.model().audioMono
            onCheckedChanged: {
                //  dccData.worker().setAudioMono(checked)
            }
        }
    }


}
