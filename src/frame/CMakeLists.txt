cmake_minimum_required(VERSION 3.7)

set(VERSION 4.0)

set(BIN_NAME dde-control-center)

# set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_FLAGS "-g -Wall")

# disable qt keywords, cause gio have signals member;
ADD_DEFINITIONS(-DQT_NO_KEYWORDS)

if (DEFINED ENABLE_MIEEE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mieee")
endif()

# Install settings
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /usr)
endif ()

set(DEFINED_LIST
DISABLE_OPACITY_ANIMATION
DISABLE_CLOUD_SYNC
DISABLE_SYS_UPDATE
DISABLE_ACCOUNT
DISABLE_DISPALY
DISABLE_DEFAULT_APPLICATIONS
DISABLE_PERSONALIZATION
DISABLE_NETWORK
DISABLE_BLUETOOTH
DISABLE_SOUND
DISABLE_DATETIME
DISABLE_POWER
DISABLE_MOUSE
DISABLE_KAYBOARD
DISABLE_WACOM
DISABLE_SYSINFO
DISABLE_NETWORK_PPPOE
DISABLE_NETWORK_AIRPLANE
DISABLE_NETWORK_VPN
DISABLE_NETWORK_PROXY
DISABLE_SYS_UPDATE_SOURCE_CHECK
DISABLE_SYS_UPDATE_MIRRORS
DISABLE_MAIN_PAGE
DCC_ENABLE_ADDOMAIN
DCC_DISABLE_KBLAYOUT
DCC_DISABLE_LANGUAGE
DCC_DISABLE_POWERSAVE
DCC_DISABLE_FEEDBACK
DCC_DISABLE_ROTATE
DCC_DISABLE_MIRACAST
DCC_DISABLE_TIMEZONE
DCC_DISABLE_GRUB
DCC_KEEP_SETTINGS_LIVE
DCC_COMMUNITY_LICENSE
)

foreach(LETTER ${DEFINED_LIST})
    if(DEFINED ${LETTER})
        add_definitions(-D${LETTER})
    endif()
endforeach()

# load files
set(FRAME_FILES
                main.cpp
                dbuscontrolcenterservice.cpp
                framecontentwrapper.cpp
#                framecontentwrapper.h
#                navigationdelegate.h
#                navigationview.h
                # quick_control/bluetooth/bluetoothdelegate.cpp
                # quick_control/bluetooth/bluetoothdelegate.h
                # quick_control/bluetooth/bluetoothlist.cpp
                # quick_control/bluetooth/bluetoothlist.h
                # quick_control/bluetooth/bluetoothlistmodel.cpp
                # quick_control/bluetooth/bluetoothlistmodel.h
                # quick_control/display/displaycontrolmodel.cpp
                # quick_control/display/displaycontrolmodel.h
                # quick_control/display/displaycontrolpage.cpp
                # quick_control/display/displaycontrolpage.h
                # quick_control/display/displayitemdelegate.cpp
                # quick_control/display/displayitemdelegate.h
                # quick_control/quickcontrolpanel.cpp
                # quick_control/quickcontrolpanel.h
                # quick_control/quickswitchbutton.cpp
                # quick_control/quickswitchbutton.h
                # quick_control/vpn/vpncontrolpage.cpp
                # quick_control/vpn/vpncontrolpage.h
                # quick_control/vpn/vpnlistdelegate.cpp
                # quick_control/vpn/vpnlistdelegate.h
                # quick_control/vpn/vpnlistmodel.cpp
                # quick_control/vpn/vpnlistmodel.h
                # quick_control/wifi/wifilistdelegate.cpp
                # quick_control/wifi/wifilistdelegate.h
                # quick_control/wifi/wifipage.cpp
                # quick_control/wifi/wifipage.h
                # quick_control/wifi/wifilistmodel.cpp
                # quick_control/wifi/wifilistmodel.h
                # quick_control/basicsettingspage.cpp
                # quick_control/basicsettingspage.h
#                navigation/navdelegate.cpp
#                navigation/navdelegate.h
#                navigation/navwidget.cpp
#                navigation/navmodel.h
#                navigation/navwidget.h
#                navigation/navmodel.cpp
#                navigationdelegate.cpp
#                notify/notifydelegate.cpp
#                notify/notifywidget.cpp
#                notify/notifydelegate.h
#                notify/notifymodel.h
#                notify/notifywidget.h
#                notify/notifyview.h
#                notify/notifyview.cpp
#                notify/notifymodel.cpp
                dbuscontrolcenterservice.h
#                frame.cpp
#                frame.h
#                framewidget.cpp
#                framewidget.h
#                indicatorwidget.cpp
#                indicatorwidget.h
#                mainwidget.cpp
#                mainwidget.h
                moduleinitthread.cpp
#                moduleinitthread.h
#                navigationbar.cpp
#                navigationbar.h
#                navigationmodel.cpp
#                navigationmodel.h
                navigationview.cpp
                pluginscontroller.cpp
#                pluginscontroller.h
#                settingswidget.cpp
#                settingswidget.h
                updatenotifier.cpp
#                updatenotifier.h
)

set(WINDOW_FILES
    window/constant.h
    window/mainwindow.cpp
    window/standarditemhelper.h
    window/standarditemhelper.cpp
    window/utils.h
    window/accessibleinterface.h
    window/accessible.h
    window/modules/display/displaywidget.cpp
    window/modules/datetime/datetimemodule.cpp
    window/modules/datetime/datetimewidget.cpp
    window/modules/datetime/datewidget.cpp
    window/modules/datetime/clockitem.cpp
    window/modules/datetime/clock.cpp
    window/modules/datetime/timezonelist.cpp
    window/modules/datetime/timezonecontentlist.cpp
    window/modules/datetime/systemtimezone.cpp
    window/modules/datetime/datesettings.cpp
    window/modules/datetime/timesetedit.cpp
    window/modules/datetime/datetimecombox.cpp
    window/modules/datetime/timespinbox.cpp
    window/modules/display/displaymodule.cpp
    window/modules/display/scalingpage.cpp
    window/modules/display/resolutiondetailpage.cpp
    window/modules/display/brightnesspage.cpp
    window/modules/display/rotatedialog.cpp
    window/modules/display/refreshratepage.cpp
    window/modules/display/multiscreensettingpage.cpp
    window/modules/display/customsettingdialog.cpp
    window/modules/display/touchpanel/screensetting.cpp
    window/modules/power/powermodule.cpp
    window/modules/power/powerwidget.cpp
    window/modules/power/generalwidget.cpp
    window/modules/power/useelectricwidget.cpp
    window/modules/power/usebatterywidget.cpp
    window/modules/sound/soundwidget.cpp
    window/modules/sound/soundmodule.cpp
    window/modules/sound/speakerpage.cpp
    window/modules/sound/microphonepage.cpp
    window/modules/sound/advancedpage.cpp
    window/modules/sound/soundeffectspage.cpp
    window/modules/update/updatemodule.cpp
    window/modules/update/updatewidget.cpp
    window/modules/update/updatectrlwidget.cpp
    window/modules/update/updatesettings.cpp
    window/modules/update/loadingitem.cpp
    window/modules/update/updatehistorybutton.cpp
    window/modules/update/recenthistoryapplist.cpp
    window/modules/update/mirrorswidget.cpp
    window/modules/update/mirrorsourceitem.cpp
    window/search/searchwidget.cpp
    window/modules/commoninfo/commoninfomodule.cpp
    window/modules/commoninfo/commoninfowidget.cpp
    window/modules/commoninfo/commoninfomodel.cpp
    window/modules/commoninfo/commoninfowork.cpp
    window/modules/commoninfo/bootwidget.cpp
    window/modules/commoninfo/commonbackgrounditem.cpp
    window/modules/commoninfo/userexperienceprogramwidget.cpp
    window/modules/wacom/wacommodule.cpp
    window/modules/wacom/wacomwidget.cpp
    window/modules/wacom/wacomworker.cpp
    window/modules/wacom/wacommodel.cpp
    window/modules/wacom/pressuresettings.cpp
    window/modules/commoninfo/developermodewidget.cpp
    window/modules/commoninfo/developermodedialog.cpp
)

# load interfaces
set(INTERFACES_FILES
                interfaces/frameproxyinterface.h
                interfaces/moduleinterface.h
                interfaces/plugininterface.h

                ../../include/interface/moduleinterface.h
                ../../include/interface/frameproxyinterface.h
)

# load widgets
set(WIDGETS_FILES
                widgets/backbutton.cpp
                widgets/basiclistdelegate.cpp
                ../../include/widgets/basiclistdelegate.h
                widgets/buttontuple.cpp
                widgets/dccslider.cpp
                ../../include/widgets/dccslider.h
                widgets/dccsliderannotated.cpp
                ../../include/widgets/dccsliderannotated.h
                ../../include/widgets/labels/largelabel.h
                ../../include/widgets/labels/normallabel.h
                ../../include/widgets/labels/smalllabel.h
                ../../include/widgets/labels/tipslabel.h
                widgets/lineeditwidget.cpp
                widgets/loadingindicator.cpp
                ../../include/widgets/loadingindicator.h
                widgets/loadingnextpagewidget.cpp
                widgets/nextbutton.cpp
                widgets/optionlistpage.cpp
                widgets/searchinput.cpp
                ../../include/widgets/searchinput.h
                widgets/separator.cpp
                ../../include/widgets/separator.h
                widgets/settingsheaderitem.cpp
                ../../include/widgets/settingsheaderitem.h
                widgets/settingsitem.cpp
                ../../include/widgets/settingsitem.h
                widgets/spinboxwidget.cpp
                ../../include/widgets/spinboxwidget.h
                widgets/tipsitem.cpp
                ../../include/widgets/tipsitem.h
                widgets/titlelabel.cpp
                ../../include/widgets/titlelabel.h
                widgets/titlevalueitem.cpp
                ../../include/widgets/titlevalueitem.h
                widgets/translucentframe.cpp
                ../../include/widgets/translucentframe.h
                widgets/settingsgroup.cpp
                widgets/switchwidget.cpp
                widgets/comboxwidget.cpp
                ../../include/widgets/comboxwidget.h
                widgets/themeitempic.cpp
                ../../include/widgets/themeitempic.h
                ../../include/widgets/utils.h
                ../../include/widgets/backbutton.h
                widgets/basiclistmodel.cpp
                ../../include/widgets/basiclistmodel.h
                widgets/basiclistview.cpp
                ../../include/widgets/basiclistview.h
                ../../include/widgets/buttontuple.h
                widgets/comboboxwidget.cpp
                ../../include/widgets/comboboxwidget.h
                widgets/contentwidget.cpp
                ../../include/widgets/contentwidget.h
                widgets/editablenextpagewidget.cpp
                ../../include/widgets/editablenextpagewidget.h
                widgets/filechoosewidget.cpp
                ../../include/widgets/filechoosewidget.h
                ../../include/widgets/lineeditwidget.h
                ../../include/widgets/loadingnextpagewidget.h
                ../../include/widgets/nextbutton.h
                widgets/nextpagewidget.cpp
                ../../include/widgets/nextpagewidget.h
                widgets/optionitem.cpp
                ../../include/widgets/optionitem.h
                ../../include/widgets/optionlistpage.h
                ../../include/widgets/settingsgroup.h
                widgets/settingshead.cpp
                ../../include/widgets/settingshead.h
                ../../include/widgets/switchwidget.h
                widgets/themeitem.cpp
                ../../include/widgets/themeitem.h
                widgets/titledslideritem.cpp
                ../../include/widgets/titledslideritem.h
                widgets/timeoutdialog.cpp
                ../../include/widgets/timeoutdialog.h
                widgets/multiselectlistview.cpp
                ../../include/widgets/multiselectlistview.h
                widgets/powerdisplaywidget.cpp
                ../../include/widgets/powerdisplaywidget.h
                widgets/widgets.qrc
)

# load modules
set(MODULE_FILES
                modules/modulewidget.h
                modules/modulewidget.cpp
                modules/modulewidgetheader.cpp
                modules/modulewidgetheader.h
                modules/moduleworker.h
)

# load accounts
set(ACCOUNTS_FILES
#                modules/accounts/accountsmodule.cpp
#                modules/accounts/avatardel.cpp
                modules/accounts/creationresult.cpp
#                modules/accounts/creationresult.h
                modules/accounts/removeuserdialog.cpp
#                modules/accounts/removeuserdialog.h
                modules/accounts/useroptionitem.cpp
                modules/accounts/widgets/fingeritem.cpp
                modules/accounts/widgets/fingeritem.h
#                modules/accounts/widgets/addialog.cpp
#                modules/accounts/widgets/addialog.h
                modules/accounts/widgets/fingerwidget.cpp
                modules/accounts/widgets/fingerwidget.h
#                modules/accounts/accountsdetailwidget.cpp
#                modules/accounts/accountsdetailwidget.h
#                modules/accounts/accountsmodule.h
#                modules/accounts/accountswidget.cpp
#                modules/accounts/accountswidget.h
                modules/accounts/accountsworker.cpp
                modules/accounts/accountsworker.h
#                modules/accounts/addfingerpage.cpp
#                modules/accounts/addfingerpage.h
#                modules/accounts/avatardel.h
                modules/accounts/avatarwidget.cpp
                modules/accounts/avatarwidget.h
#                modules/accounts/createpage.cpp
#                modules/accounts/createpage.h
                modules/accounts/fingermodel.cpp
                modules/accounts/fingermodel.h
#                modules/accounts/fingerpage.cpp
#                modules/accounts/fingerpage.h
                modules/accounts/fingerworker.cpp
                modules/accounts/fingerworker.h
#                modules/accounts/modifyavatarpage.cpp
#                modules/accounts/modifyavatarpage.h
#                modules/accounts/modifyfullnamepage.cpp
#                modules/accounts/modifyfullnamepage.h
#                modules/accounts/modifypasswordpage.cpp
#                modules/accounts/modifypasswordpage.h
                modules/accounts/user.cpp
                modules/accounts/user.h
                modules/accounts/usermodel.cpp
                modules/accounts/usermodel.h
                modules/accounts/useroptionitem.h
                # v20-accounts
                window/modules/accounts/accountsmodule.cpp
                window/modules/accounts/accountsmodule.h
                window/modules/accounts/accountswidget.cpp
                window/modules/accounts/accountswidget.h
                window/modules/accounts/createaccountpage.cpp
                window/modules/accounts/createaccountpage.h
                window/modules/accounts/modifypasswdpage.cpp
                window/modules/accounts/modifypasswdpage.h
                window/modules/accounts/accountsdetailwidget.h
                window/modules/accounts/accountsdetailwidget.cpp
                window/modules/accounts/avatarlistwidget.h
                window/modules/accounts/avatarlistwidget.cpp
                window/modules/accounts/avataritemdelegate.h
                window/modules/accounts/avataritemdelegate.cpp
                window/modules/accounts/accounntfingeitem.h
                window/modules/accounts/accounntfingeitem.cpp
                window/modules/accounts/addfingedialog.h
                window/modules/accounts/addfingedialog.cpp
                window/modules/accounts/fingerwidget.h
                window/modules/accounts/fingerwidget.cpp
)

# load bluetooth
set(BLUETOOTH_FILES
#                modules/bluetooth/bluetoothmodule.cpp
#                modules/bluetooth/pincodedialog.cpp
#                modules/bluetooth/pincodedialog.h
#                modules/bluetooth/TitleEdit.cpp
#                modules/bluetooth/TitleEdit.h
                modules/bluetooth/adapter.cpp
                modules/bluetooth/adapter.h
#                modules/bluetooth/adapterwidget.cpp
#                modules/bluetooth/adapterwidget.h
                modules/bluetooth/bluetoothmodel.cpp
                modules/bluetooth/bluetoothmodel.h
#                modules/bluetooth/bluetoothmodule.h
#                modules/bluetooth/bluetoothwidget.cpp
#                modules/bluetooth/bluetoothwidget.h
                modules/bluetooth/bluetoothworker.cpp
                modules/bluetooth/bluetoothworker.h
#                modules/bluetooth/detailpage.cpp
#                modules/bluetooth/detailpage.h
                modules/bluetooth/device.cpp
#                modules/bluetooth/device.h
#                modules/bluetooth/devicesettingsitem.cpp
#                modules/bluetooth/devicesettingsitem.h

                window/modules/bluetooth/titleedit.h
                window/modules/bluetooth/titleedit.cpp
                window/modules/bluetooth/devicesettingsitem.h
                window/modules/bluetooth/devicesettingsitem.cpp
                window/modules/bluetooth/detailpage.h
                window/modules/bluetooth/detailpage.cpp
                window/modules/bluetooth/adapterwidget.h
                window/modules/bluetooth/adapterwidget.cpp
                window/modules/bluetooth/bluetoothwidget.h
                window/modules/bluetooth/bluetoothwidget.cpp
                window/modules/bluetooth/bluetoothmodule.h
                window/modules/bluetooth/bluetoothmodule.cpp
)

#load datetime
set(DATETIME_FILES
                modules/datetime/clock.cpp
#                modules/datetime/clock.h
#                modules/datetime/clockitem.cpp
#                modules/datetime/timezone_dialog/consts.h
                modules/datetime/timezone_dialog/file_util.cpp
                modules/datetime/timezone_dialog/file_util.h
                modules/datetime/timezone_dialog/popup_menu_delegate.cpp
                modules/datetime/timezone_dialog/popup_menu_delegate.h
                modules/datetime/timezone_dialog/timezone.cpp
                modules/datetime/timezone_dialog/timezone.h
                modules/datetime/timezone_dialog/timezone_map_util.cpp
                modules/datetime/timezone_dialog/timezone_map_util.h
                modules/datetime/timezone_dialog/tooltip_pin.cpp
                modules/datetime/timezone_dialog/popup_menu.cpp
                modules/datetime/timezone_dialog/popup_menu.h
                modules/datetime/timezone_dialog/timezone_map.cpp
                modules/datetime/timezone_dialog/timezone_map.h
                modules/datetime/timezone_dialog/timezonechooser.cpp
#                modules/datetime/timezone_dialog/timezonechooser.h
#                modules/datetime/timezone_dialog/tooltip_pin.h
                modules/datetime/timezoneitem.cpp
#                modules/datetime/datetimemodule.cpp
#                modules/datetime/clockitem.h
#                modules/datetime/datesettings.cpp
#                modules/datetime/datesettings.h
#                modules/datetime/datetime.cpp
#                modules/datetime/datetime.h
                modules/datetime/datetimemodel.cpp
#                modules/datetime/datetimemodel.h
#                modules/datetime/datetimemodule.h
#                modules/datetime/datetimewidget.cpp
#                modules/datetime/datetimewidget.h
                modules/datetime/datetimework.cpp
#                modules/datetime/datetimework.h
#                modules/datetime/timezoneitem.h
)

# load defapp
set(DEFAPP_FILES
                modules/defapp/defappmodel.cpp
#                modules/defapp/defappmodel.h
#                modules/defapp/defappviewer.cpp
#                modules/defapp/defappwidget.cpp
                modules/defapp/model/category.cpp
#                modules/defapp/model/category.h
#                modules/defapp/widgets/defcategoryaddwidget.cpp
#                modules/defapp/widgets/defcategoryaddwidget.h
#                modules/defapp/widgets/funcbutton.cpp
#                modules/defapp/widgets/funcbutton.h
#                modules/defapp/widgets/optionwidget.cpp
#                modules/defapp/widgets/optionwidget.h
                modules/defapp/defappworker.cpp
#                modules/defapp/defappviewer.h
#                modules/defapp/defappwidget.h
#                modules/defapp/defappworker.h
#                modules/defapp/defaultappsmodule.cpp
#                modules/defapp/defaultappsmodule.h
#                modules/defapp/defcategorywidget.cpp
#                modules/defapp/defcategorywidget.h

                window/modules/defapp/defappwidget.cpp
                window/modules/defapp/defaultappsmodule.cpp
                window/modules/defapp/defappdetailwidget.cpp
)

# load display
set(DISPLAY_FILES
#                modules/display/connectwidget.cpp
                modules/display/iconbutton.cpp
#                modules/display/iconbutton.h
#                modules/display/miracastworker.cpp
                modules/display/monitorindicator.cpp
#                modules/display/monitorindicator.h
                modules/display/recognizedialog.cpp
#                modules/display/brightnesspage.cpp
#                modules/display/displayitemdelegate.cpp
#                modules/display/displayitemdelegate.h
#                modules/display/displaymodule.cpp
                modules/display/monitorcontrolwidget.cpp
#                modules/display/brightnessitem.cpp
#                modules/display/brightnessitem.h
#                modules/display/brightnesspage.h
#                modules/display/connectwidget.h
#                modules/display/customconfigpage.cpp
#                modules/display/customconfigpage.h
#                modules/display/displaycontrolmodel.cpp
#                modules/display/displaycontrolmodel.h
#                modules/display/displaycontrolpage.cpp
#                modules/display/displaycontrolpage.h
                modules/display/displaymodel.cpp
#                modules/display/displaymodel.h
#                modules/display/displaymodule.h
#                modules/display/displaywidget.cpp
#                modules/display/displaywidget.h
                modules/display/displayworker.cpp
#                modules/display/displayworker.h
#                modules/display/miracastdevicemodel.cpp
#                modules/display/miracastdevicemodel.h
#                modules/display/miracastitem.cpp
#                modules/display/miracastitem.h
#                modules/display/miracastmodel.cpp
#                modules/display/miracastmodel.h
#                modules/display/miracastnodevicewidget.cpp
#                modules/display/miracastnodevicewidget.h
#                modules/display/miracastsettings.cpp
#                modules/display/miracastsettings.h
#                modules/display/miracastworker.h
                modules/display/monitor.cpp
#                modules/display/monitor.h
#                modules/display/monitorcontrolwidget.h
                modules/display/monitorproxywidget.cpp
#                modules/display/monitorproxywidget.h
#                modules/display/monitorsettingdialog.cpp
#                modules/display/monitorsettingdialog.h
                modules/display/monitorsground.cpp
#                modules/display/monitorsground.h
#                modules/display/recognizedialog.h
#                modules/display/resolutiondetailpage.cpp
#                modules/display/resolutiondetailpage.h
#                modules/display/rotatedialog.cpp
#                modules/display/rotatedialog.h
#                modules/display/settingslistwidget.cpp
#                modules/display/settingslistwidget.h
#                modules/display/settingsoptionitem.cpp
#                modules/display/settingsoptionitem.h
#                modules/display/scalingpage.h
#                modules/display/scalingpage.cpp
)

# load keyboard
set(KEYBOARD_FILES
                modules/keyboard/indexdelegate.cpp
#                modules/keyboard/indexdelegate.h
                modules/keyboard/indexmodel.cpp
#                modules/keyboard/indexmodel.h
                modules/keyboard/indexview.cpp
#                modules/keyboard/keyboardcontrol.cpp
#                modules/keyboard/keyboardcontrol.h
#                modules/keyboard/keyboardwidget.cpp
#                modules/keyboard/keyitem.cpp
#                modules/keyboard/keyitem.h
#                modules/keyboard/keylabel.h
#                modules/keyboard/langwidget.cpp
                modules/keyboard/shortcutkey.cpp
#                modules/keyboard/shortcutkey.h
#                modules/keyboard/titlebuttonItem.cpp
#                modules/keyboard/keyboardmodule.cpp
                modules/keyboard/keylabel.cpp
#                modules/keyboard/checkitem.cpp
#                modules/keyboard/checkitem.h
#                modules/keyboard/customcontent.cpp
#                modules/keyboard/customcontent.h
                modules/keyboard/customedit.cpp
#                modules/keyboard/customedit.h
                modules/keyboard/customitem.cpp
#                modules/keyboard/customitem.h
                modules/keyboard/indexframe.cpp
#                modules/keyboard/indexframe.h
#                modules/keyboard/indexview.h
#                modules/keyboard/keyboarddetails.cpp
#                modules/keyboard/keyboarddetails.h
                modules/keyboard/keyboardmodel.cpp
#                modules/keyboard/keyboardmodel.h
#                modules/keyboard/keyboardmodule.h
#                modules/keyboard/keyboardwidget.h
                modules/keyboard/keyboardwork.cpp
#                modules/keyboard/keyboardwork.h
#                modules/keyboard/langwidget.h
                modules/keyboard/shortcutcontent.cpp
#                modules/keyboard/shortcutcontent.h
                modules/keyboard/shortcutitem.cpp
#                modules/keyboard/shortcutitem.h
                modules/keyboard/shortcutmodel.cpp
#                modules/keyboard/shortcutmodel.h
#                modules/keyboard/shortcutwidget.cpp
#                modules/keyboard/shortcutwidget.h
#                modules/keyboard/titlebuttonItem.h

                window/modules/keyboard/checkitem.h
                window/modules/keyboard/checkitem.cpp
                window/modules/keyboard/keyboardmodule.h
                window/modules/keyboard/keyboardmodule.cpp
                window/modules/keyboard/keyboardwidget.h
                window/modules/keyboard/keyboardwidget.cpp
                window/modules/keyboard/customcontent.h
                window/modules/keyboard/customcontent.cpp
                window/modules/keyboard/generalkbsettingwidget.h
                window/modules/keyboard/generalkbsettingwidget.cpp
                window/modules/keyboard/kblayoutsettingwidget.h
                window/modules/keyboard/kblayoutsettingwidget.cpp
                window/modules/keyboard/keyboardlayoutwidget.cpp
                window/modules/keyboard/keyboardlayoutwidget.h
                window/modules/keyboard/systemlanguagesettingwidget.h
                window/modules/keyboard/systemlanguagesettingwidget.cpp
                window/modules/keyboard/shortcutsettingwidget.h
                window/modules/keyboard/shortcutsettingwidget.cpp
                window/modules/keyboard/systemlanguagewidget.h
                window/modules/keyboard/systemlanguagewidget.cpp
)

# load mouse
set(MOUSE_FILES
#                modules/mouse/mousemodule.h
#                modules/mouse/mousemodule.cpp
#                modules/mouse/mousewidget.h
#                modules/mouse/mousewidget.cpp
#                modules/mouse/mouseworker.h
                modules/mouse/mouseworker.cpp
                modules/mouse/widget/doutestwidget.cpp
#                modules/mouse/widget/doutestwidget.h
                modules/mouse/widget/palmdetectsetting.cpp
#                modules/mouse/widget/palmdetectsetting.h
                modules/mouse/mousemodel.cpp
#                modules/mouse/mousemodel.h

                window/modules/mouse/mousemodule.h
                window/modules/mouse/mousemodule.cpp
                window/modules/mouse/mousewidget.h
                window/modules/mouse/mousewidget.cpp
                window/modules/mouse/generalsettingwidget.h
                window/modules/mouse/generalsettingwidget.cpp
                window/modules/mouse/mousesettingwidget.h
                window/modules/mouse/mousesettingwidget.cpp
                window/modules/mouse/touchpadsettingwidget.h
                window/modules/mouse/touchpadsettingwidget.cpp
                window/modules/mouse/trackpointsettingwidget.h
                window/modules/mouse/trackpointsettingwidget.cpp
)

# load network
set(NETWORK_FILES
#               modules/network/networkdetailpage.cpp
#               modules/network/connectionhotspoteditpage.h
#               modules/network/networkmodule.cpp
#               modules/network/sections/declare_metatype_for_networkmanager.h
#               modules/network/sections/abstractsection.cpp
#               modules/network/sections/abstractsection.h
#               modules/network/sections/generichotspotsection.cpp
#               modules/network/sections/genericsection.cpp
#               modules/network/sections/pppoesection.cpp
#               modules/network/sections/pppsection.cpp
#               modules/network/sections/secret8021xsection.cpp
#               modules/network/sections/secretwiredsection.cpp
#               modules/network/sections/secretwiredsection.h
#               modules/network/sections/vpn/vpnadvopenvpnsection.cpp
#               modules/network/sections/vpn/vpnadvvpncsection.cpp
#               modules/network/sections/vpn/vpnipsecsection.cpp
#               modules/network/sections/vpn/vpnopenconnectsection.cpp
#               modules/network/sections/vpn/vpnsecopenvpnsection.cpp
#               modules/network/sections/vpn/vpnsection.cpp
#               modules/network/sections/vpn/vpnstrongswansection.cpp
#               modules/network/sections/vpn/vpntlssection.cpp
#               modules/network/sections/vpn/vpnvpncsection.cpp
#               modules/network/sections/vpn/vpnsection.h
#               modules/network/sections/vpn/vpnopenconnectsection.h
#               modules/network/sections/vpn/vpntlssection.h
#               modules/network/sections/vpn/vpnopenvpnsection.h
#               modules/network/sections/vpn/vpnstrongswansection.h
#               modules/network/sections/vpn/vpnadvvpncsection.h
#               modules/network/sections/vpn/vpnproxysection.h
#               modules/network/sections/vpn/vpnsecopenvpnsection.h
#               modules/network/sections/vpn/vpnadvopenvpnsection.h
#               modules/network/sections/vpn/vpnvpncsection.h
#               modules/network/sections/vpn/vpnpppsection.h
#               modules/network/sections/vpn/vpnipsecsection.h
#               modules/network/sections/vpn/vpnopenvpnsection.cpp
#               modules/network/sections/vpn/vpnpppsection.cpp
#               modules/network/sections/vpn/vpnproxysection.cpp
#               modules/network/sections/vpn/vpnsstpproxysection.h
#               modules/network/sections/vpn/vpnsstpproxysection.cpp
#               modules/network/sections/vpn/vpnsstpsection.cpp
#               modules/network/sections/vpn/vpnsstpsection.h
#               modules/network/sections/ethernetsection.cpp
#               modules/network/sections/generichotspotsection.h
#               modules/network/sections/genericsection.h
#               modules/network/sections/secrethotspotsection.cpp
#               modules/network/sections/secretwirelesssection.cpp
#               modules/network/sections/secretwirelesssection.h
#               modules/network/sections/wirelesssection.cpp
#               modules/network/sections/pppsection.h
#               modules/network/sections/ipvxsection.h
#               modules/network/sections/ethernetsection.h
#               modules/network/sections/secrethotspotsection.h
#               modules/network/sections/pppoesection.h
#               modules/network/sections/wirelesssection.h
#               modules/network/sections/secret8021xsection.h
#               modules/network/sections/ipvxsection.cpp
#               modules/network/settings/abstractsettings.cpp
#               modules/network/settings/dslpppoesettings.cpp
#               modules/network/settings/hotspotsettings.cpp
#               modules/network/settings/vpn/vpnl2tpsettings.cpp
#               modules/network/settings/vpn/vpnopenconnectsettings.cpp
#               modules/network/settings/vpn/vpnopenvpnsettings.cpp
#               modules/network/settings/vpn/vpnpptpsettings.cpp
#               modules/network/settings/vpn/vpnstrongswansettings.cpp
#               modules/network/settings/vpn/vpnvpncsettings.cpp
#               modules/network/settings/vpn/vpnl2tpsettings.h
#               modules/network/settings/vpn/vpnopenconnectsettings.h
#               modules/network/settings/vpn/vpnopenvpnsettings.h
#               modules/network/settings/vpn/vpnpptpsettings.h
#               modules/network/settings/vpn/vpnstrongswansettings.h
#               modules/network/settings/vpn/vpnvpncsettings.h
#               modules/network/settings/vpn/vpnsstpsettings.h
#               modules/network/settings/vpn/vpnsstpsettings.cpp
#               modules/network/settings/wiredsettings.cpp
#               modules/network/settings/wirelesssettings.cpp
#               modules/network/settings/dslpppoesettings.h
#               modules/network/settings/hotspotsettings.h
#               modules/network/settings/wiredsettings.h
#               modules/network/settings/wirelesssettings.h
#               modules/network/settings/abstractsettings.h
#               modules/network/connectionvpneditpage.h
#               modules/network/connectionwirelesseditpage.h
#               modules/network/accesspointwidget.cpp
#               modules/network/accesspointwidget.h
#               modules/network/chainsproxypage.cpp
#               modules/network/chainsproxypage.h
#               modules/network/chainstypepage.cpp
#               modules/network/chainstypepage.h
#               modules/network/connectioneditpage.cpp
#               modules/network/connectioneditpage.h
#               modules/network/connectionhotspoteditpage.cpp
#               modules/network/connectionvpneditpage.cpp
#               modules/network/connectionwirelesseditpage.cpp
#               modules/network/hotspotpage.cpp
#               modules/network/hotspotpage.h
#               modules/network/networkdetailpage.h
#               modules/network/networkmodule.h
#               modules/network/networkmodulewidget.cpp
#               modules/network/networkmodulewidget.h
#               modules/network/pppoepage.cpp
#               modules/network/pppoepage.h
#               modules/network/proxypage.cpp
#               modules/network/proxypage.h
#               modules/network/vpnpage.cpp
#               modules/network/vpnpage.h
#               modules/network/wiredpage.cpp
#               modules/network/wiredpage.h
#               modules/network/wirelesspage.cpp
#               modules/network/wirelesspage.h
                window/modules/network/networkdetailpage.cpp
                window/modules/network/connectionhotspoteditpage.h
                window/modules/network/networkmodule.cpp
                window/modules/network/sections/declare_metatype_for_networkmanager.h
                window/modules/network/sections/abstractsection.cpp
                window/modules/network/sections/abstractsection.h
                window/modules/network/sections/generichotspotsection.cpp
                window/modules/network/sections/genericsection.cpp
                window/modules/network/sections/pppoesection.cpp
                window/modules/network/sections/pppsection.cpp
                window/modules/network/sections/secret8021xsection.cpp
                window/modules/network/sections/secretwiredsection.cpp
                window/modules/network/sections/secretwiredsection.h
                window/modules/network/sections/vpn/vpnadvopenvpnsection.cpp
                window/modules/network/sections/vpn/vpnadvvpncsection.cpp
                window/modules/network/sections/vpn/vpnipsecsection.cpp
                window/modules/network/sections/vpn/vpnopenconnectsection.cpp
                window/modules/network/sections/vpn/vpnsecopenvpnsection.cpp
                window/modules/network/sections/vpn/vpnsection.cpp
                window/modules/network/sections/vpn/vpnstrongswansection.cpp
                window/modules/network/sections/vpn/vpntlssection.cpp
                window/modules/network/sections/vpn/vpnvpncsection.cpp
                window/modules/network/sections/vpn/vpnsection.h
                window/modules/network/sections/vpn/vpnopenconnectsection.h
                window/modules/network/sections/vpn/vpntlssection.h
                window/modules/network/sections/vpn/vpnopenvpnsection.h
                window/modules/network/sections/vpn/vpnstrongswansection.h
                window/modules/network/sections/vpn/vpnadvvpncsection.h
                window/modules/network/sections/vpn/vpnproxysection.h
                window/modules/network/sections/vpn/vpnsecopenvpnsection.h
                window/modules/network/sections/vpn/vpnadvopenvpnsection.h
                window/modules/network/sections/vpn/vpnvpncsection.h
                window/modules/network/sections/vpn/vpnpppsection.h
                window/modules/network/sections/vpn/vpnipsecsection.h
                window/modules/network/sections/vpn/vpnopenvpnsection.cpp
                window/modules/network/sections/vpn/vpnpppsection.cpp
                window/modules/network/sections/vpn/vpnproxysection.cpp
                window/modules/network/sections/vpn/vpnsstpproxysection.h
                window/modules/network/sections/vpn/vpnsstpproxysection.cpp
                window/modules/network/sections/vpn/vpnsstpsection.cpp
                window/modules/network/sections/vpn/vpnsstpsection.h
                window/modules/network/sections/ethernetsection.cpp
                window/modules/network/sections/generichotspotsection.h
                window/modules/network/sections/genericsection.h
                window/modules/network/sections/secrethotspotsection.cpp
                window/modules/network/sections/secretwirelesssection.cpp
                window/modules/network/sections/secretwirelesssection.h
                window/modules/network/sections/wirelesssection.cpp
                window/modules/network/sections/pppsection.h
                window/modules/network/sections/ipvxsection.h
                window/modules/network/sections/ethernetsection.h
                window/modules/network/sections/secrethotspotsection.h
                window/modules/network/sections/pppoesection.h
                window/modules/network/sections/wirelesssection.h
                window/modules/network/sections/secret8021xsection.h
                window/modules/network/sections/ipvxsection.cpp
                window/modules/network/settings/abstractsettings.cpp
                window/modules/network/settings/dslpppoesettings.cpp
                window/modules/network/settings/hotspotsettings.cpp
                window/modules/network/settings/vpn/vpnl2tpsettings.cpp
                window/modules/network/settings/vpn/vpnopenconnectsettings.cpp
                window/modules/network/settings/vpn/vpnopenvpnsettings.cpp
                window/modules/network/settings/vpn/vpnpptpsettings.cpp
                window/modules/network/settings/vpn/vpnstrongswansettings.cpp
                window/modules/network/settings/vpn/vpnvpncsettings.cpp
                window/modules/network/settings/vpn/vpnl2tpsettings.h
                window/modules/network/settings/vpn/vpnopenconnectsettings.h
                window/modules/network/settings/vpn/vpnopenvpnsettings.h
                window/modules/network/settings/vpn/vpnpptpsettings.h
                window/modules/network/settings/vpn/vpnstrongswansettings.h
                window/modules/network/settings/vpn/vpnvpncsettings.h
                window/modules/network/settings/vpn/vpnsstpsettings.h
                window/modules/network/settings/vpn/vpnsstpsettings.cpp
                window/modules/network/settings/wiredsettings.cpp
                window/modules/network/settings/wirelesssettings.cpp
                window/modules/network/settings/dslpppoesettings.h
                window/modules/network/settings/hotspotsettings.h
                window/modules/network/settings/wiredsettings.h
                window/modules/network/settings/wirelesssettings.h
                window/modules/network/settings/abstractsettings.h
                window/modules/network/connectionvpneditpage.h
                window/modules/network/connectionwirelesseditpage.h
                window/modules/network/chainsproxypage.cpp
                window/modules/network/chainsproxypage.h
                window/modules/network/connectioneditpage.cpp
                window/modules/network/connectioneditpage.h
                window/modules/network/connectionhotspoteditpage.cpp
                window/modules/network/connectionvpneditpage.cpp
                window/modules/network/connectionwirelesseditpage.cpp
                window/modules/network/hotspotpage.cpp
                window/modules/network/hotspotpage.h
                window/modules/network/networkdetailpage.h
                window/modules/network/networkmodule.h
                window/modules/network/networkmodulewidget.cpp
                window/modules/network/networkmodulewidget.h
                window/modules/network/pppoepage.cpp
                window/modules/network/pppoepage.h
                window/modules/network/proxypage.cpp
                window/modules/network/proxypage.h
                window/modules/network/vpnpage.cpp
                window/modules/network/vpnpage.h
                window/modules/network/wiredpage.cpp
                window/modules/network/wiredpage.h
                window/modules/network/wirelesspage.cpp
                window/modules/network/wirelesspage.h
                window/modules/network/airplanemodepage.cpp
                window/modules/network/airplanemodepage.h
)

# load personalization
set(PERSONALIZATION_FILES
                modules/personalization/model/fontmodel.cpp
#                modules/personalization/model/fontmodel.h
                modules/personalization/model/fontsizemodel.cpp
#                modules/personalization/model/fontsizemodel.h
                modules/personalization/model/thememodel.cpp
#                modules/personalization/model/thememodel.h
#                modules/personalization/module/fontswidget/fontsettingswidget.cpp
#                modules/personalization/module/fontswidget/fontsizewidget.cpp
#                modules/personalization/module/fontswidget/fontswidget.cpp
#                modules/personalization/module/fontswidget/fontlistwidget.cpp
#                modules/personalization/module/fontswidget/fontlistwidget.h
#                modules/personalization/module/fontswidget/fontsettingswidget.h
#                modules/personalization/module/fontswidget/fontsizewidget.h
#                modules/personalization/module/fontswidget/fontswidget.h
#                modules/personalization/module/themewidget/themewidget.cpp
#                modules/personalization/module/themewidget/theme.cpp
#                modules/personalization/module/themewidget/theme.h
#                modules/personalization/module/themewidget/themewidget.h
#                modules/personalization/personalizationmodule.h
#                modules/personalization/widget/fontitem.cpp
#                modules/personalization/widget/fontitem.h
#                modules/personalization/personalizationmodule.cpp
#                modules/personalization/personalizationwidget.cpp
                modules/personalization/personalizationwork.cpp
                modules/personalization/personalizationmodel.cpp
#                modules/personalization/personalizationmodel.h
#                modules/personalization/personalizationwidget.h
#                modules/personalization/personalizationwork.h

                window/modules/personalization/personalizationmodule.h
                window/modules/personalization/personalizationmodule.cpp
                window/modules/personalization/personalizationlist.h
                window/modules/personalization/personalizationlist.cpp
                window/modules/personalization/themeitempic.h
                window/modules/personalization/themeitempic.cpp
                window/modules/personalization/roundcolorwidget.h
                window/modules/personalization/roundcolorwidget.cpp
                window/modules/personalization/personalizationgeneral.h
                window/modules/personalization/personalizationgeneral.cpp
                window/modules/personalization/perssonalizationthemewidget.h
                window/modules/personalization/perssonalizationthemewidget.cpp
                window/modules/personalization/themeitem.h
                window/modules/personalization/themeitem.cpp
                window/modules/personalization/personalizationfontswidget.cpp
                window/modules/personalization/personalizationthemelist.cpp
)

# load power
set(POWER_FILES
#                modules/power/powermodule.cpp
#                modules/power/powermodule.h
#                modules/power/powerwidget.cpp
                modules/power/powerworker.cpp
                modules/power/powermodel.cpp
#                modules/power/powermodel.h
#                modules/power/powerwidget.h
#                modules/power/powerworker.h
)

# load sound
set(SOUND_FILES
#                modules/sound/portitem.cpp
#                modules/sound/soundmodule.cpp
                modules/sound/soundworker.cpp
#                modules/sound/widgets/titledsettingsitem.cpp
#                modules/sound/widgets/titledsettingsitem.h
#                modules/sound/advancedpage.cpp
#                modules/sound/advancedpage.h
#                modules/sound/portitem.h
                modules/sound/soundmodel.cpp
#                modules/sound/soundmodel.h
#                modules/sound/soundmodule.h
#                modules/sound/soundwidget.cpp
#                modules/sound/soundwidget.h
#                modules/sound/soundworker.h
#                modules/sound/soundeffectpage.h
#                modules/sound/soundeffectpage.cpp
)

# load sync
set(SYNC_FILES
    modules/sync/syncmodel.cpp
#    modules/sync/syncmodel.h
#    modules/sync/syncwidget.cpp
#    modules/sync/syncwidget.h
    modules/sync/syncworker.cpp
#    modules/sync/syncworker.h
#    modules/sync/syncstatewidget.h
#    modules/sync/syncstatewidget.cpp
#    modules/sync/syncstateicon.h
    modules/sync/syncstateicon.cpp
#    modules/sync/utils.h

    window/modules/sync/syncmodule.h
    window/modules/sync/syncmodule.cpp
    window/modules/sync/syncwidget.h
    window/modules/sync/syncwidget.cpp
    window/modules/sync/pages/index.h
    window/modules/sync/pages/index.cpp
    window/modules/sync/pages/login.h
    window/modules/sync/pages/login.cpp
    window/modules/sync/pages/logout.cpp
    window/modules/sync/pages/downloadurl.cpp
    window/modules/sync/pages/avatarwidget.cpp
    window/modules/sync/pages/loginedin.cpp
)

# load unionid
set(UNIONID_FILES
     modules/unionid/unionidmodel.cpp
   modules/unionid/unionidworker.cpp

    window/modules/unionid/unionidmodule.h
    window/modules/unionid/unionidmodule.cpp
    window/modules/unionid/unionidwidget.h
    window/modules/unionid/unionidwidget.cpp
    window/modules/unionid/pages/index.h
    window/modules/unionid/pages/index.cpp
    window/modules/unionid/pages/login.h
    window/modules/unionid/pages/login.cpp
    window/modules/unionid/pages/logout.cpp
    window/modules/unionid/pages/downloadurl.cpp
    window/modules/unionid/pages/avatarwidget.cpp
    window/modules/unionid/pages/loginedin.cpp
)
# load systeminfo
set(SYSTEMINFO_FILES
#                modules/systeminfo/copyrightwidget.h
#                modules/systeminfo/logoitem.h
#                modules/systeminfo/copyrightwidget.cpp
                modules/systeminfo/logoitem.cpp
#                modules/systeminfo/systeminfomodule.cpp
#                modules/systeminfo/systeminfowidget.cpp
#                modules/systeminfo/userlicense.cpp
#                modules/systeminfo/userlicense.h
#                modules/systeminfo/bootwidget.h
#                modules/systeminfo/grubbackgrounditem.h
                modules/systeminfo/systeminfomodel.cpp
#                modules/systeminfo/systeminfomodel.h
#                modules/systeminfo/systeminfomodule.h
#                modules/systeminfo/systeminfowidget.h
                modules/systeminfo/systeminfowork.cpp
#                modules/systeminfo/systeminfowork.h
#                modules/systeminfo/bootwidget.cpp
#                modules/systeminfo/grubbackgrounditem.cpp
                #for V20
                window/modules/systeminfo/systeminfomodule.h
                window/modules/systeminfo/systeminfomodule.cpp
                window/modules/systeminfo/systeminfowidget.h
                window/modules/systeminfo/systeminfowidget.cpp
                window/modules/systeminfo/nativeinfowidget.h
                window/modules/systeminfo/nativeinfowidget.cpp
                window/modules/systeminfo/versionprotocolwidget.h
                window/modules/systeminfo/versionprotocolwidget.cpp
                window/modules/systeminfo/userlicensewidget.h
                window/modules/systeminfo/userlicensewidget.cpp
                window/modules/systeminfo/systemrestore.h
                window/modules/systeminfo/systemrestore.cpp
                window/modules/systeminfo/manualbackup.h
                window/modules/systeminfo/manualbackup.cpp
                window/modules/systeminfo/manualrestore.h
                window/modules/systeminfo/manualrestore.cpp
                window/modules/systeminfo/backupandrestoremodel.h
                window/modules/systeminfo/backupandrestoremodel.cpp
                window/modules/systeminfo/backupandrestoreworker.h
                window/modules/systeminfo/backupandrestoreworker.cpp
)

# load update
set(UPDATE_FILES
#                modules/update/common.h
                modules/update/loadingitem.cpp
#                modules/update/loadingitem.h
                modules/update/resultitem.cpp
#                modules/update/resultitem.h
                modules/update/summaryitem.cpp
#                modules/update/summaryitem.h
                modules/update/updateitem.cpp
#                modules/update/updateitem.h
#                modules/update/updateview.cpp
#                modules/update/updatemodule.cpp
#                modules/update/updatesettings.cpp
                modules/update/updatework.cpp
                modules/update/downloadprogressbar.cpp
#                modules/update/downloadprogressbar.h
#                modules/update/mirroritem.cpp
#                modules/update/mirroritem.h
#                modules/update/mirrorswidget.cpp
#                modules/update/mirrorswidget.h
#                modules/update/updatectrlwidget.cpp
#                modules/update/updatectrlwidget.h
                modules/update/updatemodel.cpp
#                modules/update/updatemodel.h
#                modules/update/updatemodule.h
#                modules/update/updatesettings.h
#                modules/update/updateview.h
#                modules/update/updatework.h
)

# load wacom
set(WACOM_FILES
                modules/wacom/model/wacommodelbase.cpp
#                modules/wacom/model/wacommodelbase.h
#                modules/wacom/wacommodule.cpp
#                modules/wacom/wacomwidget.cpp
                modules/wacom/wacomworker.cpp
#                modules/wacom/widget/wacomsettings.cpp
#                modules/wacom/widget/wacomsettings.h
                modules/wacom/wacommodel.cpp
#                modules/wacom/wacommodel.h
#                modules/wacom/wacommodepage.cpp
#                modules/wacom/wacommodepage.h
#                modules/wacom/wacommodule.h
#                modules/wacom/wacomwidget.h
#                modules/wacom/wacomworker.h
                #v20
#                window/modules/wacom/wacommodule.h
#                window/modules/wacom/wacommodule.cpp
#                window/modules/wacom/wacomwidget.h
#                window/modules/wacom/wacomwidget.cpp
#                window/modules/wacom/wacomworker.h
#                window/modules/wacom/wacomworker.cpp
#                window/modules/wacom/wacommodel.h
#                window/modules/wacom/wacommodel.cpp
#                window/modules/wacom/pressuresettings.h
#                window/modules/wacom/pressuresettings.cpp
)

set(NOTIFICATION_FILES
    modules/notification/notificationmodel.cpp
    modules/notification/notificationworker.cpp
    modules/notification/model/appitemmodel.cpp
    modules/notification/model/sysitemmodel.cpp
    window/modules/notification/notificationmodule.cpp
    window/modules/notification/notificationwidget.cpp
    window/modules/notification/systemnotifywidget.cpp
    window/modules/notification/notificationitem.cpp
    window/modules/notification/timeslotitem.cpp
    window/modules/notification/appnotifywidget.cpp
)

# end load

#plugins/calculator/calculatorplugin.cpp
#plugins/calculator/calculatorplugin.h
#plugins/example/exampleplugin.cpp
#plugins/example/exampleplugin.h
#plugins/weather/caiyun/caiyunlocationprovider.cpp
#plugins/weather/caiyun/caiyunlocationprovider.h
#plugins/weather/deepin/deepinlocationprovider.cpp
#plugins/weather/deepin/deepinlocationprovider.h
#plugins/weather/locationprovider.h
#plugins/weather/networkutil.cpp
#plugins/weather/networkutil.h
#plugins/weather/setlocationpage.cpp
#plugins/weather/setlocationpage.h
#plugins/weather/types.h
#plugins/weather/weatheritem.cpp
#plugins/weather/weatheritem.h
#plugins/weather/weatherplugin.cpp
#plugins/weather/weatherplugin.h
#plugins/weather/weatherrequest.cpp
#plugins/weather/weatherrequest.h
#plugins/weather/weatherwidget.h
#plugins/weather/weatherwidget.cpp

set(SRCS
        ${MODULE_FILES}
        ${FRAME_FILES}
        ${WINDOW_FILES}
        ${INTERFACES_FILES}
        ${ACCOUNTS_FILES}
        ${BLUETOOTH_FILES}
        ${DATETIME_FILES}
        ${DEFAPP_FILES}
        ${DISPLAY_FILES}
        ${KEYBOARD_FILES}
        ${MOUSE_FILES}
        ${NETWORK_FILES}
        ${SOUND_FILES}
        ${POWER_FILES}
        ${WACOM_FILES}
        ${UPDATE_FILES}
        ${SYNC_FILES}
        ${UNIONID_FILES}
        ${SYSTEMINFO_FILES}
        ${PERSONALIZATION_FILES}
        ${NOTIFICATION_FILES}
)

set(QRC
        notify/images.qrc
        frame.qrc
        modules/display/display.qrc
        modules/wacom/wacom.qrc
        modules/update/update.qrc
        modules/network/network.qrc
        modules/datetime/datetime.qrc
        modules/datetime/timezone_dialog.qrc
        modules/power/power.qrc
        modules/systeminfo/systeminfo.qrc
        modules/accounts/accounts.qrc
        modules/bluetooth/bluetooth.qrc
        modules/defapp/defapp.qrc
        modules/sound/sound.qrc
        modules/personalization/personalization.qrc
        modules/keyboard/keyboard.qrc
        modules/mouse/mouse.qrc
        modules/sync/sync.qrc
        window/search/translator/languages.qrc
        window/icons/dde-control-center-icons.qrc
        window/modules/datetime/datetime.qrc
        window/modules/update/dccv20update.qrc
)

# Find the library
find_package(PkgConfig REQUIRED)
find_package(DtkWidget REQUIRED)
find_package(DtkGui REQUIRED)
find_package(KF5NetworkManagerQt REQUIRED)
find_package(DtkCMake REQUIRED)
find_package(Qt5 COMPONENTS
    Core
    Widgets
    Concurrent
    X11Extras
    DBus
    Network
    Multimedia
REQUIRED)

pkg_check_modules(DFrameworkDBus REQUIRED dframeworkdbus)
pkg_check_modules(QGSettings REQUIRED gsettings-qt)
pkg_check_modules(DDE-Network-Utils REQUIRED dde-network-utils)
pkg_check_modules(Gio-2.0 REQUIRED gio-2.0)
pkg_check_modules(LibNM REQUIRED libnm)
pkg_search_module(UDisk2 REQUIRED udisks2-qt5)

include_directories(AFTER ${UDisk2_INCLUDE_DIRS})

set(Qt_LIBS
    Qt5::Core
    Qt5::Gui
    Qt5::DBus
    Qt5::Widgets
    Qt5::X11Extras
    Qt5::Network
    Qt5::Multimedia
)

# KF5 network manager
get_target_property(KF5_QT_INCLUDE_DIRS KF5::NetworkManagerQt INTERFACE_INCLUDE_DIRECTORIES)

set(TRY_KF5_LIBRARIES
IMPORTED_LOCATION_DEBIAN
IMPORTED_LOCATION_NOCONFIG
)

foreach(LETTER ${TRY_KF5_LIBRARIES})
    if (NOT KF5_QT_LIBRARIES)
        get_target_property(KF5_QT_LIBRARIES KF5::NetworkManagerQt ${LETTER})
    endif()
endforeach()

if (NOT KF5_QT_LIBRARIES)
    find_library(KF5_QT_LIBRARIES libKF5NetworkManagerQt.so)
endif()
# end KF5 network manager

function(add_build_macro macro)
    string(TOUPPER ${macro} macro)
    add_definitions(-D${macro})
endfunction()

set(BUILD_MACRO)

if (OS_DEEPIN_PROFESSIONAL)
    if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        set(BUILD_MACRO
            ${BUILD_MACRO}
            DCC_ENABLE_ADDOMAIN
            DCC_DISABLE_MIRACAST
            DISABLE_CLOUD_SYNC
            DCC_COMMUNITY_LICENSE
        )
    endif()

    set(BUILD_MACRO
        ${BUILD_MACRO}
    )
endif()

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "sw_64")
    set(BUILD_MACRO
        ${BUILD_MACRO}
        DISABLE_SYS_UPDATE_SOURCE_CHECK
        DISABLE_SYS_UPDATE_MIRRORS
        DCC_DISABLE_MIRACAST
        DCC_ENABLE_AUTOSTART
        DCC_DISABLE_POWERSAVE
        DCC_KEEP_SETTINGS_LIVE
        DCC_DISABLE_OPACITY_ANIMATION
        DISABLE_SYS_UPDATE_MIRRORS
        DCC_DISABLE_FEEDBACK
        DISABLE_CLOUD_SYNC
        DISABLE_SCREEN_RECORDING
    )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mieee")
endif()

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "mips64")
    set(BUILD_MACRO
        ${BUILD_MACRO}
        DISABLE_SYS_UPDATE_SOURCE_CHECK
        DCC_DISABLE_MIRACAST
        DCC_ENABLE_AUTOSTART
        DCC_DISABLE_OPACITY_ANIMATION
        DCC_KEEP_SETTINGS_LIVE
        DCC_DISABLE_GRUB_THEME
        DISABLE_CLOUD_SYNC
        DISABLE_SCREEN_RECORDING
    )
endif()

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
    set(BUILD_MACRO
        ${BUILD_MACRO}
        DISABLE_POWER
        DISABLE_CLOUD_SYNC
    )
endif()

foreach(LETTER ${BUILD_MACRO})
    add_build_macro(${LETTER})
endforeach()

add_library(dccwidgets SHARED
    ${WIDGETS_FILES}
)
target_include_directories(dccwidgets PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${DtkWidget_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
)
target_link_libraries(dccwidgets PRIVATE
    ${DtkWidget_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
)

add_executable(${BIN_NAME} ${SRCS} ${QRC})
target_include_directories(${BIN_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${DtkWidget_INCLUDE_DIRS}
    ${DFrameworkDBus_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${PROJECT_BINARY_DIR}
    ${QGSettings_INCLUDE_DIRS}
    ${DDE-Network-Utils_INCLUDE_DIRS}
    ${Gio-2.0_INCLUDE_DIRS}
    ${LibNM_INCLUDE_DIRS}
    ${Qt5X11Extras_INCLUDE_DIRS}
    ${KF5_QT_INCLUDE_DIRS}
    ${UDisk2_INCLUDE_DIRS}
)
target_link_libraries(${BIN_NAME} PRIVATE
    dccwidgets
    ${DFrameworkDBus_LIBRARIES}
    ${DtkWidget_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5Concurrent_LIBRARIES}
    ${Qt5X11Extras_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    ${Qt5Network_LIBRARIES}
    ${Qt5Multimedia_LIBRARIES}
    ${QGSettings_LIBRARIES}
    ${DDE-Network-Utils_LIBRARIES}
    ${Gio-2.0_LIBRARIES}
    ${LibNM_LIBRARIES}
    ${KF5_QT_LIBRARIES}
    crypt
    ${UDisk2_LIBRARIES}
)

# bin
install(TARGETS ${BIN_NAME} DESTINATION bin)
install(TARGETS dccwidgets LIBRARY DESTINATION lib)

find_package(Qt5 COMPONENTS
Test
REQUIRED)

set(Qt_LIBS
${Qt_LIBS}
Qt5::Test
)

#set(TEST_SRCS
#    tests/display_test.h
#    ${DISPLAY_FILES}
#    ${WIDGETS_FILES}
#    ${MODULE_FILES}
#)

#add_executable(display-unit-test
#    ${TEST_SRCS}
#)

#target_include_directories(display-unit-test PUBLIC
#    ${DtkWidget_INCLUDE_DIRS}
#    ${DFrameworkDBus_INCLUDE_DIRS}
#    ${QGSettings_INCLUDE_DIRS}
#    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
#    ${Qt5X11Extras_INCLUDE_DIRS}
#    ${QGSettings_INCLUDE_DIRS}
#    ${XCB_EWMH_INCLUDE_DIRS}
#    ${PROJECT_BINARY_DIR}
#)

#target_link_libraries(display-unit-test PRIVATE
#    ${DISPLAY_TEST_LIBS}
#    ${DFrameworkDBus_LIBRARIES}
#    ${DtkWidget_LIBRARIES}
#    ${Qt5Widgets_LIBRARIES}
#    ${Qt5X11Extras_LIBRARIES}
#    ${Qt5DBus_LIBRARIES}
#    ${QGSettings_LIBRARIES}
#    ${XCB_EWMH_LIBRARIES}
#    ${Qt_LIBS}
#)

#add_test(NAME display-unit-test COMMAND "${CMAKE_CURRENT_BINARY_DIR}/display-unit-test")
