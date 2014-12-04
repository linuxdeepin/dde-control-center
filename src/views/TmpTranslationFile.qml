import QtQuick 2.1

Item {
    property string tmpName:""

    Component.onCompleted: {
        //显示模式
        tmpName = dsTr("Display Modes")

        //复制

        tmpName = dsTr("Copy")

        //将您的一个屏幕内容复制到其他屏幕

        tmpName = dsTr("Copy the contents of your primary screen to other screens.")

        //扩展

        tmpName = dsTr("Extend")

        //将您的屏幕内容扩展，在不同屏幕显示不同内容

        tmpName = dsTr("Extend your screen contents. Different contents will be displayed on different screens.")

        //仅在VGA1上显示

        tmpName = dsTr("Only Displayed on %1").arg("VGA1")

        //仅在VGA1上显示屏幕内容，其他屏幕不显示

        tmpName = dsTr("Screen contents are only displayed on %1 but not on other screens.").arg("VGA1")

        //自定义设置

        tmpName = dsTr("Custom Settings")

        //您可以对屏幕进行其他自定义设置

        tmpName = dsTr("You can do other custom settings to your screens.")

        //双击此区域以更改时间

        tmpName = dsTr("Double-click this area to change your time")

        //正在准备账户信息…

        tmpName = dsTr("Account information is being prepared...")

        //主屏

        tmpName = dsTr("Primary Screen")
    }
}
