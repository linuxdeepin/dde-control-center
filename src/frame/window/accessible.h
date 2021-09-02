/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     fpc_diesel <fanpengcheng@uniontech.com>
 *
 * Maintainer: fpc_diesel <fanpengcheng@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ACCESSIBLE_H
#define ACCESSIBLE_H

#include "accessibleinterface.h"

#include "./mainwindow.h"
#include "./search/searchwidget.h"

#include "./modules/personalization/personalizationlist.h"
#include "./modules/personalization/personalizationgeneral.h"
#include "./modules/personalization/personalizationthemelist.h"
#include "./modules/personalization/personalizationfontswidget.h"
#include "./modules/personalization/themeitem.h"
#include "./modules/personalization/themeitempic.h"
#include "./modules/personalization/roundcolorwidget.h"
#include "./modules/personalization/perssonalizationthemewidget.h"

#include "./modules/accounts/avatarwidget.h"
#include "./modules/accounts/createaccountpage.h"
#include "./modules/accounts/accountsdetailwidget.h"
#include "./modules/accounts/avatarwidget.h"

#include "./modules/bluetooth/bluetoothwidget.h"
#include "./modules/bluetooth/detailpage.h"
#include "./modules/bluetooth/adapterwidget.h"

#include "./modules/commoninfo/commoninfowidget.h"
#include "./modules/commoninfo/developermodewidget.h"
#include "./modules/commoninfo/userexperienceprogramwidget.h"
#include "./modules/commoninfo/bootwidget.h"

#include "./modules/datetime/datetimewidget.h"
#include "./modules/datetime/systemtimezone.h"
#include "./modules/datetime/datesettings.h"
#include "./modules/datetime/timezoneitem.h"
#include "./modules/datetime/datewidget.h"
#include "./modules/datetime/clockitem.h"
#include "./modules/datetime/timezone_dialog/timezonechooser.h"

#include "./modules/defapp/defappwidget.h"
#include "./modules/defapp/defappdetailwidget.h"

#include "./modules/display/displaywidget.h"
#include "./modules/display/scalingwidget.h"
#include "./modules/display/resolutionwidget.h"
#include "./modules/display/brightnesswidget.h"
#include "./modules/display/refreshratewidget.h"

#include "./modules/touchscreen/touchscreenwidget.h"

#include "./modules/keyboard/keyboardwidget.h"
#include "./modules/keyboard/generalkbsettingwidget.h"
#include "./modules/keyboard/kblayoutsettingwidget.h"
#include "./modules/keyboard/systemlanguagewidget.h"
#include "./modules/keyboard/shortcutsettingwidget.h"
#include "./modules/keyboard/shortcutitem.h"

#include "./modules/mouse/mousewidget.h"
#include "./modules/mouse/generalsettingwidget.h"
#include "./modules/mouse/mousesettingwidget.h"
#include "./modules/mouse/touchpadsettingwidget.h"
#include "./modules/mouse/trackpointsettingwidget.h"
#include "./modules/mouse/widget/doutestwidget.h"

#include "./modules/notification/notificationwidget.h"
#include "./modules/notification/systemnotifywidget.h"
#include "./modules/notification/appnotifywidget.h"

#include "./modules/power/powerwidget.h"
#include "./modules/power/generalwidget.h"
#include "./modules/power/useelectricwidget.h"
#include "./modules/power/usebatterywidget.h"

#include "./modules/sound/soundwidget.h"
#include "./modules/sound/microphonepage.h"
#include "./modules/sound/speakerpage.h"
#include "./modules/sound/soundeffectspage.h"

#include "./modules/sync/syncwidget.h"

#include "./modules/systeminfo/systeminfowidget.h"
#include "./modules/systeminfo/nativeinfowidget.h"
#include "./modules/systeminfo/versionprotocolwidget.h"
#include "./modules/systeminfo/userlicensewidget.h"

#include "./modules/update/updatewidget.h"

#include "./modules/wacom/wacomwidget.h"

#include "./widgets/multiselectlistview.h"
#include "./widgets/settingsitem.h"
#include "./widgets/dccslider.h"
#include "./widgets/settingsitem.h"
#include "./widgets/settingsgroup.h"
#include "./widgets/settingshead.h"
#include "./widgets/titledslideritem.h"

#include <QPushButton>
#include <QWidget>
#include <QLineEdit>
#include <QToolButton>
#include <QSlider>
#include <QFrame>
#include <QMenu>
#include <QLabel>

#include <DSwitchButton>
#include <DBackgroundGroup>
#include <DFloatingButton>
#include <DLineEdit>
#include <DLabel>
#include <DListView>
#include <DCommandLinkButton>
#include <DSearchEdit>
#include <DTitlebar>
#include <DComboBox>
/**************************************************************************************/
DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::search;

// 主窗口
SET_FORM_ACCESSIBLE(MainWindow, "mainwindow")
SET_FORM_ACCESSIBLE(SearchWidget, "searchwidget")

// 账户模块
using namespace DCC_NAMESPACE::accounts;
using namespace dcc::accounts;

SET_FORM_ACCESSIBLE(AccountsWidget, "accountswidget")
SET_FORM_ACCESSIBLE(CreateAccountPage, "createaccountpage")
SET_FORM_ACCESSIBLE(AccountsDetailWidget, "accountsdetailwidget")
SET_FORM_ACCESSIBLE(AvatarWidget, "avatarwidget")

// 蓝牙模块
using namespace DCC_NAMESPACE::bluetooth;

SET_FORM_ACCESSIBLE(DetailPage, "detailpage")
SET_FORM_ACCESSIBLE(BluetoothWidget, "bluetoothwidget")
SET_FORM_ACCESSIBLE(AdapterWidget, "adapterwidget")

// 通用模块
using namespace DCC_NAMESPACE::commoninfo;

SET_FORM_ACCESSIBLE(CommonInfoWidget, "commoninfowidget")
SET_FORM_ACCESSIBLE(DeveloperModeWidget, "developermodewidget")
SET_FORM_ACCESSIBLE(UserExperienceProgramWidget, "userexperienceprogramwidget")
SET_FORM_ACCESSIBLE(BootWidget, "bootwidget")

// 时钟模块
using namespace DCC_NAMESPACE::datetime;
using TimezoneItem = dcc::datetime::TimezoneItem;
using TimeZoneChooser = dcc::datetime::TimeZoneChooser;

SET_FORM_ACCESSIBLE(DatetimeWidget, "datetimewidget")
SET_FORM_ACCESSIBLE(SystemTimezone, "systemtimezone")
SET_FORM_ACCESSIBLE(DateSettings, "datesettings")
SET_FORM_ACCESSIBLE(TimezoneItem, "timezoneitem")
SET_FORM_ACCESSIBLE(DateWidget, "datewidget")
SET_FORM_ACCESSIBLE(ClockItem, "clockitem")
SET_FORM_ACCESSIBLE(TimeZoneChooser, "TimeZoneChooser")

// 默认程序模块
using DefaultAppsWidget = DCC_NAMESPACE::defapp::DefaultAppsWidget;
using DefappDetailWidget = DCC_NAMESPACE::defapp::DefappDetailWidget;

SET_FORM_ACCESSIBLE(DefaultAppsWidget, "DefaultAppsWidget")
SET_FORM_ACCESSIBLE(DefappDetailWidget, "DefappDetailWidget")

// 屏幕模块
using namespace DCC_NAMESPACE::display;
using namespace dcc::widgets;

SET_FORM_ACCESSIBLE(DisplayWidget, "DisplayWidget")
SET_FORM_ACCESSIBLE(ScalingWidget, "ScalingWidget")
SET_FORM_ACCESSIBLE(ResolutionWidget, "ResolutionWidget")
SET_FORM_ACCESSIBLE(BrightnessWidget, "BrightnessWidget")
SET_FORM_ACCESSIBLE(RefreshRateWidget, "RefreshRateWidget")

// 键盘模块
using namespace DCC_NAMESPACE::keyboard;
using namespace dcc::keyboard;

SET_FORM_ACCESSIBLE(KeyboardWidget, "KeyboardWidget")
SET_FORM_ACCESSIBLE(GeneralKBSettingWidget, "GeneralKBSettingWidget")
SET_FORM_ACCESSIBLE(KBLayoutSettingWidget, "KBLayoutSettingWidget")
SET_FORM_ACCESSIBLE(SystemLanguageWidget, "SystemLanguageWidget")
SET_FORM_ACCESSIBLE(ShortCutSettingWidget, "ShortCutSettingWidget")
SET_FORM_ACCESSIBLE(ShortcutItem, "ShortcutItem")

// 鼠标模块
using namespace DCC_NAMESPACE::mouse;
using namespace dcc::mouse;

SET_FORM_ACCESSIBLE(MouseWidget, "MouseWidget")
SET_FORM_ACCESSIBLE(GeneralSettingWidget, "GeneralSettingWidget")
SET_FORM_ACCESSIBLE(MouseSettingWidget, "MouseSettingWidget")
SET_FORM_ACCESSIBLE(TouchPadSettingWidget, "TouchPadSettingWidget")
SET_FORM_ACCESSIBLE(TrackPointSettingWidget, "TrackPointSettingWidget")
SET_FORM_ACCESSIBLE(DouTestWidget, "DouTestWidget")

// 电源模块
using namespace DCC_NAMESPACE::power;

SET_FORM_ACCESSIBLE(PowerWidget, "PowerWidget")
SET_FORM_ACCESSIBLE(GeneralWidget, "GeneralWidget")
SET_FORM_ACCESSIBLE(UseElectricWidget, "UseElectricWidget")
SET_FORM_ACCESSIBLE(UseBatteryWidget, "UseBatteryWidget")

// 通知模块
using namespace DCC_NAMESPACE::notification;
SET_FORM_ACCESSIBLE(NotificationWidget, "NotificationWidget");
SET_FORM_ACCESSIBLE(SystemNotifyWidget, "SystemNotifyWidget");
SET_FORM_ACCESSIBLE(AppNotifyWidget, "AppNotifyWidget");

// 声音模块
using namespace DCC_NAMESPACE::sound;

SET_FORM_ACCESSIBLE(SoundWidget, "SoundWidget")
SET_FORM_ACCESSIBLE(SpeakerPage, "SpeakerPage")
SET_FORM_ACCESSIBLE(MicrophonePage, "MicrophonePage")
SET_FORM_ACCESSIBLE(SoundEffectsPage, "SoundEffectsPage")

// 网络账户
using namespace DCC_NAMESPACE::sync;

SET_FORM_ACCESSIBLE(SyncWidget, "SyncWidget")

// 系统信息模块
using namespace DCC_NAMESPACE::systeminfo;

SET_FORM_ACCESSIBLE(VersionProtocolWidget, "VersionProtocolWidget")
SET_FORM_ACCESSIBLE(SystemInfoWidget, "SystemInfoWidget")
SET_FORM_ACCESSIBLE(NativeInfoWidget, "NativeInfoWidget")
SET_FORM_ACCESSIBLE(UserLicenseWidget, "UserLicenseWidget")

// 更新模块
using namespace DCC_NAMESPACE::update;

SET_FORM_ACCESSIBLE(UpdateWidget, "UpdateWidget")

// 手写板模块
using namespace DCC_NAMESPACE::wacom;

SET_FORM_ACCESSIBLE(WacomWidget, "WacomWidget")

// 个性化模块
using namespace DCC_NAMESPACE::personalization;

SET_FORM_ACCESSIBLE(PersonalizationList, "PersonalizationList")
SET_FORM_ACCESSIBLE(PersonalizationGeneral, "PersonalizationGeneral")
SET_FORM_ACCESSIBLE(PerssonalizationThemeWidget, "PerssonalizationThemeWidget")
SET_FORM_ACCESSIBLE(ThemeItem, "ThemeItem")
SET_BUTTON_ACCESSIBLE(ThemeItemPic, "ThemeItemPic")
SET_FORM_ACCESSIBLE(RoundColorWidget, "RoundColorWidget")

// dcc控件
using namespace dcc::widgets;

SET_FORM_ACCESSIBLE(SettingsGroup, "SettingsGroup")
SET_FORM_ACCESSIBLE(SettingsHead, "SettingsHead")
SET_FORM_ACCESSIBLE(SwitchWidget, "SwitchWidget")
SET_FORM_ACCESSIBLE(SettingsItem, "SettingsItem")
SET_FORM_ACCESSIBLE(DCCSlider, "DCCSlider");
SET_FORM_ACCESSIBLE(TitledSliderItem, "TitledSliderItem");


// Qt控件
SET_FORM_ACCESSIBLE(QWidget, m_w->objectName().isEmpty() ? "widget" : m_w->objectName())
SET_BUTTON_ACCESSIBLE(QPushButton, m_w->text().isEmpty() ? "qpushbutton" : m_w->text())
SET_EDITABLE_ACCESSIBLE(QLineEdit, m_w->text().isEmpty() ? "qlineedit" : m_w->text())
SET_BUTTON_ACCESSIBLE(QToolButton, m_w->text().isEmpty() ? "qtoolbutton" : m_w->text())
SET_SLIDER_ACCESSIBLE(QSlider, "qslider")
SET_FORM_ACCESSIBLE(QMenu, "qmenu")
SET_LABEL_ACCESSIBLE(QLabel, m_w->text().isEmpty() ? "qlabel" : m_w->text())

// Dtk控件
SET_FORM_ACCESSIBLE(DBackgroundGroup, m_w->objectName().isEmpty() ? "dbackgroundgroup" : m_w->objectName())
SET_BUTTON_ACCESSIBLE(DSwitchButton, m_w->text().isEmpty() ? "switchbutton" : m_w->text())
SET_BUTTON_ACCESSIBLE(DFloatingButton,  m_w->toolTip().isEmpty() ? "DFloatingButton" : m_w->toolTip())
SET_DTK_EDITABLE_ACCESSIBLE(DLineEdit, "DLineEdit")
SET_FORM_ACCESSIBLE(DSearchEdit, m_w->objectName().isEmpty() ? "DSearchEdit" : m_w->objectName())
SET_BUTTON_ACCESSIBLE(DIconButton, m_w->objectName().isEmpty() ? "DIconButton" : m_w->objectName())
SET_BUTTON_ACCESSIBLE(DCommandLinkButton, "DCommandLinkButton")
SET_LABEL_ACCESSIBLE(DLabel, m_w->text().isEmpty() ? m_w->objectName().isEmpty() ? "DLabel" : m_w->objectName() : m_w->text())
SET_FORM_ACCESSIBLE(DTitlebar, m_w->objectName().isEmpty() ? "DTitlebar" : m_w->objectName())
SET_SLIDER_ACCESSIBLE(DSlider, "dslider")


QAccessibleInterface *accessibleFactory(const QString &classname, QObject *object)
{
    QAccessibleInterface *interface = nullptr;

    // 应用主窗口
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), MainWindow);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::search::", ""), SearchWidget);
    USE_ACCESSIBLE_BY_OBJECTNAME(classname, QWidget, "contentwindow");
    USE_ACCESSIBLE_BY_OBJECTNAME(classname, QWidget, "AvatarItem");
    USE_ACCESSIBLE_BY_OBJECTNAME(QString(classname).replace("Dtk::Widget::", ""), DBackgroundGroup, "modulepage");

    // 账户模块
    USE_ACCESSIBLE(QString(classname).replace("dccV20::accounts::", ""), AccountsWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::accounts::", ""), CreateAccountPage);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::accounts::", ""), AccountsDetailWidget);
    USE_ACCESSIBLE(QString(classname).replace("dcc::accounts::", ""), AvatarWidget);

    // 蓝牙模块
    USE_ACCESSIBLE(QString(classname).replace("dccV20::bluetooth::", ""), BluetoothWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::bluetooth::", ""), AdapterWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::bluetooth::", ""), DetailPage);

    // 通用模块
    USE_ACCESSIBLE(QString(classname).replace("dccV20::commoninfo::", ""), CommonInfoWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::commoninfo::", ""), DeveloperModeWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::commoninfo::", ""), UserExperienceProgramWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::commoninfo::", ""), BootWidget);

    // 时间模块
    USE_ACCESSIBLE(QString(classname).replace("dccV20::datetime::", ""), DatetimeWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::datetime::", ""), SystemTimezone);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::datetime::", ""), DateSettings);
    USE_ACCESSIBLE(QString(classname).replace("dcc::datetime::", ""), TimezoneItem);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::datetime::", ""), DateWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::datetime::", ""), ClockItem);
    USE_ACCESSIBLE(QString(classname).replace("dcc::datetime::", ""), TimeZoneChooser);

    // 默认程序模块
    USE_ACCESSIBLE(QString(classname).replace("dccV20::defapp::", ""), DefaultAppsWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::defapp::", ""), DefappDetailWidget);

    // 屏幕模块
    USE_ACCESSIBLE(QString(classname).replace("dccV20::display::", ""), DisplayWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::display::", ""), ScalingWidget);
    USE_ACCESSIBLE(QString(classname).replace("dcc::widgets::", ""), TitledSliderItem);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::display::", ""), ResolutionWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::display::", ""), BrightnessWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::display::", ""), RefreshRateWidget);

    // 键盘模块
    USE_ACCESSIBLE(QString(classname).replace("dccV20::keyboard::", ""), KeyboardWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::keyboard::", ""), GeneralKBSettingWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::keyboard::", ""), KBLayoutSettingWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::keyboard::", ""), SystemLanguageWidget);
    USE_ACCESSIBLE(QString(classname).replace("dcc::keyboard::", ""), ShortcutItem);

    // 鼠标模块
    USE_ACCESSIBLE(QString(classname).replace("dccV20::mouse::", ""), MouseWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::mouse::", ""), GeneralSettingWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::mouse::", ""), MouseSettingWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::mouse::", ""), TouchPadSettingWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::mouse::", ""), TrackPointSettingWidget);
    USE_ACCESSIBLE(QString(classname).replace("dcc::mouse::", ""), DouTestWidget);

    // 个性化模块
    USE_ACCESSIBLE(QString(classname).replace("dccV20::personalization::", ""), PersonalizationList);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::personalization::", ""), PersonalizationGeneral);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::personalization::", ""), PerssonalizationThemeWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::personalization::", ""), ThemeItem);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::personalization::", ""), ThemeItemPic);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::personalization::", ""), RoundColorWidget);

    // 电源模块
    USE_ACCESSIBLE(QString(classname).replace("dccV20::power::", ""), PowerWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::power::", ""), GeneralWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::power::", ""), UseElectricWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::power::", ""), UseBatteryWidget);

    // 声音模块
    USE_ACCESSIBLE(QString(classname).replace("dccV20::sound::", ""), SoundWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::sound::", ""), SpeakerPage);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::sound::", ""), MicrophonePage);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::sound::", ""), SoundEffectsPage);

    // 网络账户模块
    USE_ACCESSIBLE(QString(classname).replace("dccV20::sync::", ""), SyncWidget);

    // 系统信息模块
    USE_ACCESSIBLE(QString(classname).replace("dccV20::systeminfo::", ""), VersionProtocolWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::systeminfo::", ""), SystemInfoWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::systeminfo::", ""), NativeInfoWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::systeminfo::", ""), UserLicenseWidget);

    // 更新模块
    USE_ACCESSIBLE(QString(classname).replace("dccV20::update::", ""), UpdateWidget);

    // 手写板模块
    USE_ACCESSIBLE(QString(classname).replace("dccV20::wacom::", ""), WacomWidget);

    // dcc控件
    USE_ACCESSIBLE(QString(classname).replace("dcc::widgets::", ""), SettingsGroup);
    USE_ACCESSIBLE(QString(classname).replace("dcc::widgets::", ""), SettingsHead);
    USE_ACCESSIBLE(QString(classname).replace("dcc::widgets::", ""), SwitchWidget);
    USE_ACCESSIBLE(QString(classname).replace("dcc::widgets::", ""), SettingsItem);
    USE_ACCESSIBLE(QString(classname).replace("dcc::widgets::", ""), DCCSlider);

    // Qt控件
    USE_ACCESSIBLE(classname, QPushButton);
    USE_ACCESSIBLE(classname, QToolButton);
    USE_ACCESSIBLE(classname, QLineEdit);
    USE_ACCESSIBLE(classname, QSlider);
    USE_ACCESSIBLE(classname, QMenu);
    USE_ACCESSIBLE(classname, QLabel);

    // Dtk控件
    USE_ACCESSIBLE(QString(classname).replace("Dtk::Widget::", ""), DLineEdit);
    USE_ACCESSIBLE(QString(classname).replace("Dtk::Widget::", ""), DSwitchButton);
    USE_ACCESSIBLE(QString(classname).replace("Dtk::Widget::", ""), DIconButton);
    USE_ACCESSIBLE(QString(classname).replace("Dtk::Widget::", ""), DCommandLinkButton);
    USE_ACCESSIBLE(QString(classname).replace("Dtk::Widget::", ""), DSearchEdit);
    USE_ACCESSIBLE(QString(classname).replace("Dtk::Widget::", ""), DTitlebar);
    USE_ACCESSIBLE(QString(classname).replace("Dtk::Widget::", ""), DLabel);
    USE_ACCESSIBLE(QString(classname).replace("dcc::widgets::", ""), DSwitchButton);
    USE_ACCESSIBLE(QString(classname).replace("dcc::widgets::", ""), DSlider);

    return interface;
}
#endif // ACCESSIBLE_H
