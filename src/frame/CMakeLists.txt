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
DCC_ENABLE_END_USER_LICENSE
DCC_KEEP_SETTINGS_LIVE
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
                framecontentwrapper.h
                navigationdelegate.h
                navigationview.h
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
                navigation/navdelegate.cpp
                navigation/navdelegate.h
                navigation/navwidget.cpp
                navigation/navmodel.h
                navigation/navwidget.h
                navigation/navmodel.cpp
                navigationdelegate.cpp
                notify/notifydelegate.cpp
                notify/notifywidget.cpp
                notify/notifydelegate.h
                notify/notifymodel.h
                notify/notifywidget.h
                notify/notifyview.h
                notify/notifyview.cpp
                notify/notifymodel.cpp
                dbuscontrolcenterservice.h
                frame.cpp
                frame.h
                framewidget.cpp
                framewidget.h
                indicatorwidget.cpp
                indicatorwidget.h
                mainwidget.cpp
                mainwidget.h
                moduleinitthread.cpp
                moduleinitthread.h
                navigationbar.cpp
                navigationbar.h
                navigationmodel.cpp
                navigationmodel.h
                navigationview.cpp
                pluginscontroller.cpp
                pluginscontroller.h
                settingswidget.cpp
                settingswidget.h
                updatenotifier.cpp
                updatenotifier.h
)

# load interfaces
set(INTERFACES_FILES
                interfaces/frameproxyinterface.h
                interfaces/moduleinterface.h
                interfaces/plugininterface.h
)

# load widgets
set(WIDGETS_FILES
                widgets/backbutton.cpp
                widgets/basiclistdelegate.cpp
                widgets/basiclistdelegate.h
                widgets/buttontuple.cpp
                widgets/dccslider.cpp
                widgets/dccslider.h
                widgets/dccsliderannotated.cpp
                widgets/dccsliderannotated.h
                widgets/labels/largelabel.h
                widgets/labels/normallabel.h
                widgets/labels/smalllabel.h
                widgets/labels/tipslabel.h
                widgets/lineeditwidget.cpp
                widgets/loadingindicator.cpp
                widgets/loadingindicator.h
                widgets/loadingnextpagewidget.cpp
                widgets/nextbutton.cpp
                widgets/optionlistpage.cpp
                widgets/plantextitem.cpp
                widgets/plantextitem.h
                widgets/searchinput.cpp
                widgets/searchinput.h
                widgets/separator.cpp
                widgets/separator.h
                widgets/settingsheaderitem.cpp
                widgets/settingsheaderitem.h
                widgets/settingsitem.cpp
                widgets/settingsitem.h
                widgets/spinboxwidget.cpp
                widgets/spinboxwidget.h
                widgets/tipsitem.cpp
                widgets/tipsitem.h
                widgets/titlevalueitem.cpp
                widgets/titlevalueitem.h
                widgets/translucentframe.cpp
                widgets/translucentframe.h
                widgets/settingsgroup.cpp
                widgets/passwdeditwidget.cpp
                widgets/switchwidget.cpp
                widgets/themeitempic.cpp
                widgets/themeitempic.h
                widgets/utils.h
                widgets/backbutton.h
                widgets/basiclistmodel.cpp
                widgets/basiclistmodel.h
                widgets/basiclistview.cpp
                widgets/basiclistview.h
                widgets/buttontuple.h
                widgets/comboboxwidget.cpp
                widgets/comboboxwidget.h
                widgets/contentwidget.cpp
                widgets/contentwidget.h
                widgets/editablenextpagewidget.cpp
                widgets/editablenextpagewidget.h
                widgets/filechoosewidget.cpp
                widgets/filechoosewidget.h
                widgets/lineeditwidget.h
                widgets/loadingnextpagewidget.h
                widgets/nextbutton.h
                widgets/nextpagewidget.cpp
                widgets/nextpagewidget.h
                widgets/optionitem.cpp
                widgets/optionitem.h
                widgets/optionlistpage.h
                widgets/passwdeditwidget.h
                widgets/settingsgroup.h
                widgets/settingshead.cpp
                widgets/settingshead.h
                widgets/switchwidget.h
                widgets/themeitem.cpp
                widgets/themeitem.h
                widgets/titledslideritem.cpp
                widgets/titledslideritem.h
                widgets/timeoutdialog.cpp
                widgets/timeoutdialog.h
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
                modules/accounts/accountsmodule.cpp
                modules/accounts/avatardel.cpp
                modules/accounts/creationresult.cpp
                modules/accounts/creationresult.h
                modules/accounts/removeuserdialog.cpp
                modules/accounts/removeuserdialog.h
                modules/accounts/useroptionitem.cpp
                modules/accounts/widgets/fingeritem.cpp
                modules/accounts/widgets/fingeritem.h
                modules/accounts/widgets/addialog.cpp
                modules/accounts/widgets/addialog.h
                modules/accounts/widgets/fingerwidget.cpp
                modules/accounts/widgets/fingerwidget.h
                modules/accounts/accountsdetailwidget.cpp
                modules/accounts/accountsdetailwidget.h
                modules/accounts/accountsmodule.h
                modules/accounts/accountswidget.cpp
                modules/accounts/accountswidget.h
                modules/accounts/accountsworker.cpp
                modules/accounts/accountsworker.h
                modules/accounts/addfingerpage.cpp
                modules/accounts/addfingerpage.h
                modules/accounts/avatardel.h
                modules/accounts/avatarwidget.cpp
                modules/accounts/avatarwidget.h
                modules/accounts/createpage.cpp
                modules/accounts/createpage.h
                modules/accounts/fingermodel.cpp
                modules/accounts/fingermodel.h
                modules/accounts/fingerpage.cpp
                modules/accounts/fingerpage.h
                modules/accounts/fingerworker.cpp
                modules/accounts/fingerworker.h
                modules/accounts/modifyavatarpage.cpp
                modules/accounts/modifyavatarpage.h
                modules/accounts/modifyfullnamepage.cpp
                modules/accounts/modifyfullnamepage.h
                modules/accounts/modifypasswordpage.cpp
                modules/accounts/modifypasswordpage.h
                modules/accounts/user.cpp
                modules/accounts/user.h
                modules/accounts/usermodel.cpp
                modules/accounts/usermodel.h
                modules/accounts/useroptionitem.h
)

# load bluetooth
set(BLUETOOTH_FILES
                modules/bluetooth/bluetoothmodule.cpp
                modules/bluetooth/pincodedialog.cpp
                modules/bluetooth/pincodedialog.h
                modules/bluetooth/TitleEdit.cpp
                modules/bluetooth/TitleEdit.h
                modules/bluetooth/adapter.cpp
                modules/bluetooth/adapter.h
                modules/bluetooth/adapterwidget.cpp
                modules/bluetooth/adapterwidget.h
                modules/bluetooth/bluetoothmodel.cpp
                modules/bluetooth/bluetoothmodel.h
                modules/bluetooth/bluetoothmodule.h
                modules/bluetooth/bluetoothwidget.cpp
                modules/bluetooth/bluetoothwidget.h
                modules/bluetooth/bluetoothworker.cpp
                modules/bluetooth/bluetoothworker.h
                modules/bluetooth/detailpage.cpp
                modules/bluetooth/detailpage.h
                modules/bluetooth/device.cpp
                modules/bluetooth/device.h
                modules/bluetooth/devicesettingsitem.cpp
                modules/bluetooth/devicesettingsitem.h
)

#load datetime
set(DATETIME_FILES
                modules/datetime/clock.cpp
                modules/datetime/clock.h
                modules/datetime/clockitem.cpp
                modules/datetime/timezone_dialog/consts.h
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
                modules/datetime/timezone_dialog/timezonechooser.h
                modules/datetime/timezone_dialog/tooltip_pin.h
                modules/datetime/timezoneitem.cpp
                modules/datetime/datetimemodule.cpp
                modules/datetime/clockitem.h
                modules/datetime/datesettings.cpp
                modules/datetime/datesettings.h
                modules/datetime/datetime.cpp
                modules/datetime/datetime.h
                modules/datetime/datetimemodel.cpp
                modules/datetime/datetimemodel.h
                modules/datetime/datetimemodule.h
                modules/datetime/datetimewidget.cpp
                modules/datetime/datetimewidget.h
                modules/datetime/datetimework.cpp
                modules/datetime/datetimework.h
                modules/datetime/timezoneitem.h
)

# load defapp
set(DEFAPP_FILES
                modules/defapp/defappmodel.cpp
                modules/defapp/defappmodel.h
                modules/defapp/defappviewer.cpp
                modules/defapp/defappwidget.cpp
                modules/defapp/model/category.cpp
                modules/defapp/model/category.h
                modules/defapp/widgets/defcategoryaddwidget.cpp
                modules/defapp/widgets/defcategoryaddwidget.h
                modules/defapp/widgets/funcbutton.cpp
                modules/defapp/widgets/funcbutton.h
                modules/defapp/widgets/optionwidget.cpp
                modules/defapp/widgets/optionwidget.h
                modules/defapp/defappworker.cpp
                modules/defapp/defappviewer.h
                modules/defapp/defappwidget.h
                modules/defapp/defappworker.h
                modules/defapp/defaultappsmodule.cpp
                modules/defapp/defaultappsmodule.h
                modules/defapp/defcategorywidget.cpp
                modules/defapp/defcategorywidget.h
)

# load display
set(DISPLAY_FILES
                modules/display/connectwidget.cpp
                modules/display/iconbutton.cpp
                modules/display/iconbutton.h
                modules/display/miracastworker.cpp
                modules/display/monitorindicator.cpp
                modules/display/monitorindicator.h
                modules/display/recognizedialog.cpp
                modules/display/brightnesspage.cpp
                modules/display/displayitemdelegate.cpp
                modules/display/displayitemdelegate.h
                modules/display/displaymodule.cpp
                modules/display/monitorcontrolwidget.cpp
                modules/display/brightnessitem.cpp
                modules/display/brightnessitem.h
                modules/display/brightnesspage.h
                modules/display/connectwidget.h
                modules/display/customconfigpage.cpp
                modules/display/customconfigpage.h
                modules/display/displaycontrolmodel.cpp
                modules/display/displaycontrolmodel.h
                modules/display/displaycontrolpage.cpp
                modules/display/displaycontrolpage.h
                modules/display/displaymodel.cpp
                modules/display/displaymodel.h
                modules/display/displaymodule.h
                modules/display/displaywidget.cpp
                modules/display/displaywidget.h
                modules/display/displayworker.cpp
                modules/display/displayworker.h
                modules/display/miracastdevicemodel.cpp
                modules/display/miracastdevicemodel.h
                modules/display/miracastitem.cpp
                modules/display/miracastitem.h
                modules/display/miracastmodel.cpp
                modules/display/miracastmodel.h
                modules/display/miracastnodevicewidget.cpp
                modules/display/miracastnodevicewidget.h
                modules/display/miracastsettings.cpp
                modules/display/miracastsettings.h
                modules/display/miracastworker.h
                modules/display/monitor.cpp
                modules/display/monitor.h
                modules/display/monitorcontrolwidget.h
                modules/display/monitorproxywidget.cpp
                modules/display/monitorproxywidget.h
                modules/display/monitorsettingdialog.cpp
                modules/display/monitorsettingdialog.h
                modules/display/monitorsground.cpp
                modules/display/monitorsground.h
                modules/display/recognizedialog.h
                modules/display/resolutiondetailpage.cpp
                modules/display/resolutiondetailpage.h
                modules/display/rotatedialog.cpp
                modules/display/rotatedialog.h
                modules/display/settingslistwidget.cpp
                modules/display/settingslistwidget.h
                modules/display/settingsoptionitem.cpp
                modules/display/settingsoptionitem.h
                modules/display/scalingpage.h
                modules/display/scalingpage.cpp
)

# load keyboard
set(KEYBOARD_FILES
                modules/keyboard/indexdelegate.cpp
                modules/keyboard/indexdelegate.h
                modules/keyboard/indexmodel.cpp
                modules/keyboard/indexmodel.h
                modules/keyboard/indexview.cpp
                modules/keyboard/keyboardcontrol.cpp
                modules/keyboard/keyboardcontrol.h
                modules/keyboard/keyboardwidget.cpp
                modules/keyboard/keyitem.cpp
                modules/keyboard/keyitem.h
                modules/keyboard/keylabel.h
                modules/keyboard/langwidget.cpp
                modules/keyboard/shortcutkey.cpp
                modules/keyboard/shortcutkey.h
                modules/keyboard/titlebuttonItem.cpp
                modules/keyboard/keyboardmodule.cpp
                modules/keyboard/keylabel.cpp
                modules/keyboard/checkitem.cpp
                modules/keyboard/checkitem.h
                modules/keyboard/customcontent.cpp
                modules/keyboard/customcontent.h
                modules/keyboard/customedit.cpp
                modules/keyboard/customedit.h
                modules/keyboard/customitem.cpp
                modules/keyboard/customitem.h
                modules/keyboard/indexframe.cpp
                modules/keyboard/indexframe.h
                modules/keyboard/indexview.h
                modules/keyboard/keyboarddetails.cpp
                modules/keyboard/keyboarddetails.h
                modules/keyboard/keyboardlayoutwidget.cpp
                modules/keyboard/keyboardlayoutwidget.h
                modules/keyboard/keyboardmodel.cpp
                modules/keyboard/keyboardmodel.h
                modules/keyboard/keyboardmodule.h
                modules/keyboard/keyboardwidget.h
                modules/keyboard/keyboardwork.cpp
                modules/keyboard/keyboardwork.h
                modules/keyboard/langwidget.h
                modules/keyboard/shortcutcontent.cpp
                modules/keyboard/shortcutcontent.h
                modules/keyboard/shortcutitem.cpp
                modules/keyboard/shortcutitem.h
                modules/keyboard/shortcutmodel.cpp
                modules/keyboard/shortcutmodel.h
                modules/keyboard/shortcutwidget.cpp
                modules/keyboard/shortcutwidget.h
                modules/keyboard/titlebuttonItem.h
)

# load mouse
set(MOUSE_FILES
                modules/mouse/mousemodule.cpp
                modules/mouse/mousemodule.h
                modules/mouse/mousewidget.cpp
                modules/mouse/mouseworker.cpp
                modules/mouse/widget/doutestwidget.cpp
                modules/mouse/widget/doutestwidget.h
                modules/mouse/widget/palmdetectsetting.cpp
                modules/mouse/widget/palmdetectsetting.h
                modules/mouse/mousemodel.cpp
                modules/mouse/mousemodel.h
                modules/mouse/mousewidget.h
                modules/mouse/mouseworker.h
)

# load network
set(NETWORK_FILES
                modules/network/networkdetailpage.cpp
                modules/network/connectionhotspoteditpage.h
                modules/network/networkmodule.cpp
                modules/network/sections/declare_metatype_for_networkmanager.h
                modules/network/sections/abstractsection.cpp
                modules/network/sections/abstractsection.h
                modules/network/sections/generichotspotsection.cpp
                modules/network/sections/genericsection.cpp
                modules/network/sections/pppoesection.cpp
                modules/network/sections/pppsection.cpp
                modules/network/sections/secret8021xsection.cpp
                modules/network/sections/secretwiredsection.cpp
                modules/network/sections/secretwiredsection.h
                modules/network/sections/vpn/vpnadvopenvpnsection.cpp
                modules/network/sections/vpn/vpnadvvpncsection.cpp
                modules/network/sections/vpn/vpnipsecsection.cpp
                modules/network/sections/vpn/vpnopenconnectsection.cpp
                modules/network/sections/vpn/vpnsecopenvpnsection.cpp
                modules/network/sections/vpn/vpnsection.cpp
                modules/network/sections/vpn/vpnstrongswansection.cpp
                modules/network/sections/vpn/vpntlssection.cpp
                modules/network/sections/vpn/vpnvpncsection.cpp
                modules/network/sections/vpn/vpnsection.h
                modules/network/sections/vpn/vpnopenconnectsection.h
                modules/network/sections/vpn/vpntlssection.h
                modules/network/sections/vpn/vpnopenvpnsection.h
                modules/network/sections/vpn/vpnstrongswansection.h
                modules/network/sections/vpn/vpnadvvpncsection.h
                modules/network/sections/vpn/vpnproxysection.h
                modules/network/sections/vpn/vpnsecopenvpnsection.h
                modules/network/sections/vpn/vpnadvopenvpnsection.h
                modules/network/sections/vpn/vpnvpncsection.h
                modules/network/sections/vpn/vpnpppsection.h
                modules/network/sections/vpn/vpnipsecsection.h
                modules/network/sections/vpn/vpnopenvpnsection.cpp
                modules/network/sections/vpn/vpnpppsection.cpp
                modules/network/sections/vpn/vpnproxysection.cpp
                modules/network/sections/vpn/vpnsstpproxysection.h
                modules/network/sections/vpn/vpnsstpproxysection.cpp
                modules/network/sections/vpn/vpnsstpsection.cpp
                modules/network/sections/vpn/vpnsstpsection.h
                modules/network/sections/ethernetsection.cpp
                modules/network/sections/generichotspotsection.h
                modules/network/sections/genericsection.h
                modules/network/sections/secrethotspotsection.cpp
                modules/network/sections/secretwirelesssection.cpp
                modules/network/sections/secretwirelesssection.h
                modules/network/sections/wirelesssection.cpp
                modules/network/sections/pppsection.h
                modules/network/sections/ipvxsection.h
                modules/network/sections/ethernetsection.h
                modules/network/sections/secrethotspotsection.h
                modules/network/sections/pppoesection.h
                modules/network/sections/wirelesssection.h
                modules/network/sections/secret8021xsection.h
                modules/network/sections/ipvxsection.cpp
                modules/network/settings/abstractsettings.cpp
                modules/network/settings/dslpppoesettings.cpp
                modules/network/settings/hotspotsettings.cpp
                modules/network/settings/vpn/vpnl2tpsettings.cpp
                modules/network/settings/vpn/vpnopenconnectsettings.cpp
                modules/network/settings/vpn/vpnopenvpnsettings.cpp
                modules/network/settings/vpn/vpnpptpsettings.cpp
                modules/network/settings/vpn/vpnstrongswansettings.cpp
                modules/network/settings/vpn/vpnvpncsettings.cpp
                modules/network/settings/vpn/vpnl2tpsettings.h
                modules/network/settings/vpn/vpnopenconnectsettings.h
                modules/network/settings/vpn/vpnopenvpnsettings.h
                modules/network/settings/vpn/vpnpptpsettings.h
                modules/network/settings/vpn/vpnstrongswansettings.h
                modules/network/settings/vpn/vpnvpncsettings.h
                modules/network/settings/vpn/vpnsstpsettings.h
                modules/network/settings/vpn/vpnsstpsettings.cpp
                modules/network/settings/wiredsettings.cpp
                modules/network/settings/wirelesssettings.cpp
                modules/network/settings/dslpppoesettings.h
                modules/network/settings/hotspotsettings.h
                modules/network/settings/wiredsettings.h
                modules/network/settings/wirelesssettings.h
                modules/network/settings/abstractsettings.h
                modules/network/connectionvpneditpage.h
                modules/network/connectionwirelesseditpage.h
                modules/network/accesspointwidget.cpp
                modules/network/accesspointwidget.h
                modules/network/chainsproxypage.cpp
                modules/network/chainsproxypage.h
                modules/network/chainstypepage.cpp
                modules/network/chainstypepage.h
                modules/network/connectioneditpage.cpp
                modules/network/connectioneditpage.h
                modules/network/connectionhotspoteditpage.cpp
                modules/network/connectionvpneditpage.cpp
                modules/network/connectionwirelesseditpage.cpp
                modules/network/hotspotpage.cpp
                modules/network/hotspotpage.h
                modules/network/networkdetailpage.h
                modules/network/networkmodule.h
                modules/network/networkmodulewidget.cpp
                modules/network/networkmodulewidget.h
                modules/network/pppoepage.cpp
                modules/network/pppoepage.h
                modules/network/proxypage.cpp
                modules/network/proxypage.h
                modules/network/vpnpage.cpp
                modules/network/vpnpage.h
                modules/network/wiredpage.cpp
                modules/network/wiredpage.h
                modules/network/wirelesspage.cpp
                modules/network/wirelesspage.h
)

# load personalization
set(PERSONALIZATION_FILES
                modules/personalization/model/fontmodel.cpp
                modules/personalization/model/fontmodel.h
                modules/personalization/model/fontsizemodel.cpp
                modules/personalization/model/fontsizemodel.h
                modules/personalization/model/thememodel.cpp
                modules/personalization/model/thememodel.h
                modules/personalization/module/fontswidget/fontsettingswidget.cpp
                modules/personalization/module/fontswidget/fontsizewidget.cpp
                modules/personalization/module/fontswidget/fontswidget.cpp
                modules/personalization/module/fontswidget/fontlistwidget.cpp
                modules/personalization/module/fontswidget/fontlistwidget.h
                modules/personalization/module/fontswidget/fontsettingswidget.h
                modules/personalization/module/fontswidget/fontsizewidget.h
                modules/personalization/module/fontswidget/fontswidget.h
                modules/personalization/module/themewidget/themewidget.cpp
                modules/personalization/module/themewidget/theme.cpp
                modules/personalization/module/themewidget/theme.h
                modules/personalization/module/themewidget/themewidget.h
                modules/personalization/personalizationmodule.h
                modules/personalization/widget/fontitem.cpp
                modules/personalization/widget/fontitem.h
                modules/personalization/personalizationmodule.cpp
                modules/personalization/personalizationwidget.cpp
                modules/personalization/personalizationwork.cpp
                modules/personalization/personalizationmodel.cpp
                modules/personalization/personalizationmodel.h
                modules/personalization/personalizationwidget.h
                modules/personalization/personalizationwork.h
)

# load power
set(POWER_FILES
                modules/power/powermodule.cpp
                modules/power/powermodule.h
                modules/power/powerwidget.cpp
                modules/power/powerworker.cpp
                modules/power/powermodel.cpp
                modules/power/powermodel.h
                modules/power/powerwidget.h
                modules/power/powerworker.h
)

# load sound
set(SOUND_FILES
                modules/sound/portitem.cpp
                modules/sound/soundmodule.cpp
                modules/sound/soundworker.cpp
                modules/sound/widgets/titledsettingsitem.cpp
                modules/sound/widgets/titledsettingsitem.h
                modules/sound/advancedpage.cpp
                modules/sound/advancedpage.h
                modules/sound/portitem.h
                modules/sound/soundmodel.cpp
                modules/sound/soundmodel.h
                modules/sound/soundmodule.h
                modules/sound/soundwidget.cpp
                modules/sound/soundwidget.h
                modules/sound/soundworker.h
                modules/sound/soundeffectpage.h
                modules/sound/soundeffectpage.cpp
)

# load sync
set(SYNC_FILES
    modules/sync/syncmodel.cpp
    modules/sync/syncmodel.h
    modules/sync/syncmodule.cpp
    modules/sync/syncmodule.h
    modules/sync/syncwidget.cpp
    modules/sync/syncwidget.h
    modules/sync/syncworker.cpp
    modules/sync/syncworker.h
    modules/sync/syncstatewidget.h
    modules/sync/syncstatewidget.cpp
    modules/sync/syncstateicon.h
    modules/sync/syncstateicon.cpp
)

# load systeminfo
set(SYSTEMINFO_FILES
                modules/systeminfo/copyrightwidget.h
                modules/systeminfo/logoitem.h
                modules/systeminfo/copyrightwidget.cpp
                modules/systeminfo/logoitem.cpp
                modules/systeminfo/systeminfomodule.cpp
                modules/systeminfo/systeminfowidget.cpp
                modules/systeminfo/userlicense.cpp
                modules/systeminfo/userlicense.h
                modules/systeminfo/bootwidget.h
                modules/systeminfo/grubbackgrounditem.h
                modules/systeminfo/systeminfomodel.cpp
                modules/systeminfo/systeminfomodel.h
                modules/systeminfo/systeminfomodule.h
                modules/systeminfo/systeminfowidget.h
                modules/systeminfo/systeminfowork.cpp
                modules/systeminfo/systeminfowork.h
                modules/systeminfo/bootwidget.cpp
                modules/systeminfo/grubbackgrounditem.cpp
)

# load update
set(UPDATE_FILES
                modules/update/common.h
                modules/update/loadingitem.cpp
                modules/update/loadingitem.h
                modules/update/resultitem.cpp
                modules/update/resultitem.h
                modules/update/summaryitem.cpp
                modules/update/summaryitem.h
                modules/update/updateitem.cpp
                modules/update/updateitem.h
                modules/update/updateview.cpp
                modules/update/updatemodule.cpp
                modules/update/updatesettings.cpp
                modules/update/updatework.cpp
                modules/update/downloadprogressbar.cpp
                modules/update/downloadprogressbar.h
                modules/update/mirroritem.cpp
                modules/update/mirroritem.h
                modules/update/mirrorswidget.cpp
                modules/update/mirrorswidget.h
                modules/update/updatectrlwidget.cpp
                modules/update/updatectrlwidget.h
                modules/update/updatemodel.cpp
                modules/update/updatemodel.h
                modules/update/updatemodule.h
                modules/update/updatesettings.h
                modules/update/updateview.h
                modules/update/updatework.h
)

# load wacom
set(WACOM_FILES
                modules/wacom/model/wacommodelbase.cpp
                modules/wacom/model/wacommodelbase.h
                modules/wacom/wacommodule.cpp
                modules/wacom/wacomwidget.cpp
                modules/wacom/wacomworker.cpp
                modules/wacom/widget/wacomsettings.cpp
                modules/wacom/widget/wacomsettings.h
                modules/wacom/wacommodel.cpp
                modules/wacom/wacommodel.h
                modules/wacom/wacommodepage.cpp
                modules/wacom/wacommodepage.h
                modules/wacom/wacommodule.h
                modules/wacom/wacomwidget.h
                modules/wacom/wacomworker.h
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
        ${INTERFACES_FILES}
        ${WIDGETS_FILES}
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
        ${SYSTEMINFO_FILES}
        ${PERSONALIZATION_FILES}
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
        widgets/widgets.qrc
        modules/sync/sync.qrc
)

# Find the library
find_package(PkgConfig REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5X11Extras REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(DtkWidget REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(KF5NetworkManagerQt REQUIRED)
find_package(DtkCMake REQUIRED)

pkg_check_modules(XCB_EWMH REQUIRED xcb-ewmh x11 xext)
pkg_check_modules(DFrameworkDBus REQUIRED dframeworkdbus)
pkg_check_modules(QGSettings REQUIRED gsettings-qt)
pkg_check_modules(DDE-Network-Utils REQUIRED dde-network-utils)
pkg_check_modules(Gio-2.0 REQUIRED gio-2.0)
pkg_check_modules(LibNM REQUIRED libnm)

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
            DISABLE_SYS_UPDATE_SOURCE_CHECK
            DISABLE_SYS_UPDATE_MIRRORS
            DCC_DISABLE_MIRACAST
            DISABLE_CLOUD_SYNC
        )
    endif()

    set(BUILD_MACRO
        ${BUILD_MACRO}
        DCC_ENABLE_END_USER_LICENSE
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
        DCC_DISABLE_GRUB
        DISABLE_SYS_UPDATE_MIRRORS
        DCC_DISABLE_FEEDBACK
        DISABLE_CLOUD_SYNC
    )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mieee")
endif()

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "mips64")
    set(BUILD_MACRO
        ${BUILD_MACRO}
        DISABLE_SYS_UPDATE_SOURCE_CHECK
        DISABLE_SYS_UPDATE_MIRRORS
        DCC_DISABLE_MIRACAST
        DCC_ENABLE_AUTOSTART
        DCC_DISABLE_OPACITY_ANIMATION
        DCC_KEEP_SETTINGS_LIVE
        DCC_DISABLE_GRUB
        DCC_DISABLE_POWERSAVE
        DISABLE_CLOUD_SYNC
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

add_executable(${BIN_NAME} ${SRCS} ${QRC})
target_include_directories(${BIN_NAME} PUBLIC
    ${DtkWidget_INCLUDE_DIRS}
    ${XCB_EWMH_INCLUDE_DIRS}
    ${DFrameworkDBus_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${PROJECT_BINARY_DIR}
    ${QGSettings_INCLUDE_DIRS}
    ${DDE-Network-Utils_INCLUDE_DIRS}
    ${Gio-2.0_INCLUDE_DIRS}
    ${LibNM_INCLUDE_DIRS}
    ${Qt5X11Extras_INCLUDE_DIRS}
    ${KF5_QT_INCLUDE_DIRS}
)

target_link_libraries(${BIN_NAME} PRIVATE
    ${XCB_EWMH_LIBRARIES}
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
)

# bin
install(TARGETS ${BIN_NAME} DESTINATION bin)
