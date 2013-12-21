import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../widgets"


DBaseExpand {
    property variant dev
    property variant accessPoints : nm.GetAccessPoints(dev[0])
    id: c2
    width: root.width
    expanded: dev[1] != 20
    header.sourceComponent: DSwitchButtonHeader {
        text: dsTr("Wireless Access Points" + dev[1])
        active: c2.expanded
        onActiveChanged: {
            nm.wirelessEnabled = active
        }
    }
    content.sourceComponent: Column {
        Repeater {
            model: c2.accessPoints
            delegate: Column {
                width: root.width
                DBaseLine {
                    MouseArea{
                        z:-1
                        anchors.fill:parent
                        onClicked: {
                            print("HUHU")
                            nm.ActiveAccessPoint(dev[0], accessPoints[index][3])
                        }
                    }
                    color: dconstants.contentBgColor
                    leftLoader.sourceComponent: Row {
                        DImageButton {
                            visible: accessPoints[index][4]
                            normal_image: "img/check_1.png"
                            hover_image: "img/check_2.png"
                            onClicked: {
                                print("Try disconnect", dev[0])
                                nm.DisconnectDevice(dev[0])
                            }
                        }
                        DLabel {
                            verticalAlignment: Text.AlignVCenter
                            text: accessPoints[index][0] + " :" + accessPoints[index][3].substring(44)
                        }
                    }

                    rightLoader.sourceComponent: Row {
                        Image {
                            source: {
                                var power=  accessPoints[index][2]
                                if (power <= 20)
                                    return "img/wifi_1.png"
                                else if (power <= 50)
                                    return "img/wifi_2.png"
                                else if (power <= 80)
                                    return "img/wifi_3.png"
                                else if (power <= 100)
                                    return "img/wifi_4.png"
                            }
                        }
                        DLabel {
                            text: accessPoints[index][2] + " " + (accessPoints[index][1] ? "!" : " ")
                        }
                        DImageButton {
                            normal_image: "../widgets/images/arrow_right_normal.png"
                            hover_image: "../widgets/images/arrow_right_hover.png"
                            press_image: "../widgets/images/arrow_right_press.png"
                            onClicked: {
                                print("X")
                            }
                        }
                    }
                }
                DBaseLine {
                    id:password
                    property string conn
                    visible: false
                    leftLoader.sourceComponent: DLabel {
                        text: dsTr("PassWord")
                    }
                    rightLoader.sourceComponent: TextField {
                        width: 200
                        onAccepted: {
                            print("Try Active...")
                            visible= false
                            nm.SetKey(password.conn,  text)
                        }
                    }
                    Connections {
                        target: nm
                        onNeedMoreConfigure: {
                            password.conn = nm.GetConnectionByAccessPoint(accessPoints[index][3])[1]
                            if (password.conn == arg0) {
                                password.visible = true
                                print("HAHAHAHAHAHAH", parent)
                            } else {
                                print("HandleNeedMoreConfigure:", arg0, arg1, password.conn)
                            }
                        }
                    }
                }
                DSeparatorHorizontal{}
            }
        }
        Timer {
            running: c2.expanded
            interval: 1000
            onTriggered : {
                c2.accessPoints = nm.GetAccessPoints(dev[0])
            }
        }
    }
}
