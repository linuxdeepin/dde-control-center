#include "gtest/gtest.h"
#include "src/plugin-keyboard/operation/shortcutmodel.h"
#include "src/plugin-keyboard/window/shortcutsettingwidget.h"

#include <DFloatingButton>

#include <QLineEdit>
#include <QSignalSpy>

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

namespace dcc {
namespace keyboard {
class ShortcutModel;
class ShortcutItem;
} // namespace keyboard
} // namespace dcc

class Tst_ShortCutSettingWidget : public testing::Test
{
    void SetUp() override;

    void TearDown() override;

public:
    ShortCutSettingWidget *widget = nullptr;
    ShortcutModel *model = nullptr;
};

void Tst_ShortCutSettingWidget::SetUp()
{
    model = new ShortcutModel();
    widget = new ShortCutSettingWidget(model);
}

void Tst_ShortCutSettingWidget::TearDown()
{
    delete model;
    model = nullptr;
    delete widget;
    widget = nullptr;
}

TEST_F(Tst_ShortCutSettingWidget, emitSignal)
{
    QString str(
            "[{\"Id\":\"reload\",\"Type\":2,\"Accels\":[\"XF86Reload\"],\"Name\":\"Reload\"},{"
            "\"Id\":\"wlan\",\"Type\":2,\"Accels\":[\"XF86WLAN\"],\"Name\":\"WLAN\"},{\"Id\":"
            "\"move-to-workspace-11\",\"Type\":3,\"Accels\":[],\"Name\":\"Move to workspace "
            "11\"},{\"Id\":\"move-to-workspace-9\",\"Type\":3,\"Accels\":[],\"Name\":\"Move to "
            "workspace "
            "9\"},{\"Id\":\"switch-to-workspace-4\",\"Type\":3,\"Accels\":[\"<Super>4\"],\"Name\":"
            "\"Switch to workspace "
            "4\"},{\"Id\":\"open\",\"Type\":2,\"Accels\":[\"XF86Open\"],\"Name\":\"Open\"},{\"Id\":"
            "\"rotate-windows\",\"Type\":2,\"Accels\":[\"XF86RotateWindows\"],\"Name\":\"rotate-"
            "windows\"},{\"Id\":\"suspend\",\"Type\":2,\"Accels\":[\"XF86Suspend\"],\"Name\":"
            "\"suspend\"},{\"Id\":\"maximize-horizontally\",\"Type\":3,\"Accels\":[],\"Name\":"
            "\"Maximize window "
            "horizontally\"},{\"Id\":\"xfer\",\"Type\":2,\"Accels\":[\"XF86Xfer\"],\"Name\":"
            "\"xfer\"},{\"Id\":\"my-computer\",\"Type\":2,\"Accels\":[\"XF86MyComputer\"],\"Name\":"
            "\"MyComputer\"},{\"Id\":\"save\",\"Type\":2,\"Accels\":[\"XF86Save\"],\"Name\":"
            "\"Save\"},{\"Id\":\"display\",\"Type\":2,\"Accels\":[\"XF86Display\"],\"Name\":"
            "\"Display\"},{\"Id\":\"begin-move\",\"Type\":3,\"Accels\":[\"<Alt>F7\"],\"Name\":"
            "\"移动窗口\"},{\"Id\":\"file-manager\",\"Type\":0,\"Accels\":[\"<Super>E\"],\"Name\":"
            "\"文件管理器\"},{\"Id\":\"away\",\"Type\":2,\"Accels\":[],\"Name\":\"Away\"},{\"Id\":"
            "\"close\",\"Type\":2,\"Accels\":[\"XF86Close\"],\"Name\":\"Close\"},{\"Id\":\"mail-"
            "forward\",\"Type\":2,\"Accels\":[\"XF86MailForward\"],\"Name\":\"mail-forward\"},{"
            "\"Id\":\"move-to-workspace-right\",\"Type\":3,\"Accels\":[\"<Shift><Super>Right\"],"
            "\"Name\":\"移动到右边工作区\"},{\"Id\":\"preview-workspace\",\"Type\":3,\"Accels\":["
            "\"<Super>S\"],\"Name\":\"显示工作区 "
            "\"},{\"Id\":\"switch-to-workspace-6\",\"Type\":3,\"Accels\":[\"<Super>6\"],\"Name\":"
            "\"Switch to workspace "
            "6\"},{\"Id\":\"eject\",\"Type\":2,\"Accels\":[\"XF86Eject\"],\"Name\":\"eject\"},{"
            "\"Id\":\"audio-mute\",\"Type\":2,\"Accels\":[\"XF86AudioMute\"],\"Name\":"
            "\"AudioMute\"},{\"Id\":\"screenshot-delayed\",\"Type\":0,\"Accels\":[\"<Control>"
            "Print\"],\"Name\":\"延时截图\"},{\"Id\":\"scroll-down\",\"Type\":2,\"Accels\":["
            "\"XF86ScrollDown\"],\"Name\":\"scroll-down\"},{\"Id\":\"audio-pause\",\"Type\":2,"
            "\"Accels\":[\"XF86AudioPause\"],\"Name\":\"AudioPause\"},{\"Id\":\"expose-all-"
            "windows\",\"Type\":3,\"Accels\":[\"<Super>A\"],\"Name\":\"显示所有工作区的窗口  "
            "\"},{\"Id\":\"switch-to-workspace-8\",\"Type\":3,\"Accels\":[\"<Super>8\"],\"Name\":"
            "\"Switch to workspace "
            "8\"},{\"Id\":\"unmaximize\",\"Type\":3,\"Accels\":[\"<Super>Down\"],\"Name\":"
            "\"恢复窗口\"},{\"Id\":\"translation\",\"Type\":0,\"Accels\":[\"<Control><Alt>U\"],"
            "\"Name\":\"文本翻译\"},{\"Id\":\"switch-group\",\"Type\":3,\"Accels\":[\"<Alt>grave\"]"
            ",\"Name\":\"切换同类型窗口\"},{\"Id\":\"logout\",\"Type\":0,\"Accels\":[\"<Control><"
            "Alt>Delete\",\"<Control><Alt>KP_Delete\"],\"Name\":\"关机界面\"},{\"Id\":\"sleep\","
            "\"Type\":2,\"Accels\":[\"XF86Sleep\"],\"Name\":\"Sleep\"},{\"Id\":\"touchpad-off\","
            "\"Type\":2,\"Accels\":[\"XF86TouchpadOff\"],\"Name\":\"touchpad-off\"},{\"Id\":"
            "\"cut\",\"Type\":2,\"Accels\":[\"XF86Cut\"],\"Name\":\"Cut\"},{\"Id\":\"move-to-"
            "workspace-6\",\"Type\":3,\"Accels\":[],\"Name\":\"Move to workspace "
            "6\"},{\"Id\":\"move-to-workspace-left\",\"Type\":3,\"Accels\":[\"<Shift><Super>Left\"]"
            ",\"Name\":\"移动到左边工作区\"},{\"Id\":\"switch-to-workspace-7\",\"Type\":3,"
            "\"Accels\":[\"<Super>7\"],\"Name\":\"Switch to workspace "
            "7\"},{\"Id\":\"switch-monitors\",\"Type\":2,\"Accels\":[\"<Super>P\"],\"Name\":"
            "\"多屏切换\"},{\"Id\":\"capslock\",\"Type\":2,\"Accels\":[\"Caps_Lock\"],\"Name\":"
            "\"capslock\"},{\"Id\":\"favorites\",\"Type\":2,\"Accels\":[\"XF86Favorites\"],"
            "\"Name\":\"Favorites\"},{\"Id\":\"switch-to-workspace-left\",\"Type\":3,\"Accels\":["
            "\"<Super>Left\"],\"Name\":\"切换到左边工作区\"},{\"Id\":\"launcher\",\"Type\":0,"
            "\"Accels\":[\"Super_L\",\"Super_R\"],\"Name\":\"启动器\"},{\"Id\":\"forward\","
            "\"Type\":2,\"Accels\":[\"XF86Forward\"],\"Name\":\"Forward\"},{\"Id\":\"shop\","
            "\"Type\":2,\"Accels\":[\"XF86Shop\"],\"Name\":\"shop\"},{\"Id\":\"terminal\",\"Type\":"
            "0,\"Accels\":[\"<Control><Alt>T\"],\"Name\":\"终端\"},{\"Id\":\"kbd-brightness-up\","
            "\"Type\":2,\"Accels\":[\"XF86KbdBrightnessUp\"],\"Name\":\"kbd-brightness-up\"},{"
            "\"Id\":\"audio-stop\",\"Type\":2,\"Accels\":[],\"Name\":\"AudioStop\"},{\"Id\":"
            "\"game\",\"Type\":2,\"Accels\":[\"XF86Game\"],\"Name\":\"Game\"},{\"Id\":\"copy\","
            "\"Type\":2,\"Accels\":[\"XF86Copy\"],\"Name\":\"Copy\"},{\"Id\":\"de91f94f-cf30-4799-"
            "9b06-d1c03b1b6045\",\"Type\":1,\"Accels\":[\"<Control>bracketright\"],\"Name\":"
            "\"aaa\",\"Exec\":\"aaa\"},{\"Id\":\"phone\",\"Type\":2,\"Accels\":[\"XF86Phone\"],"
            "\"Name\":\"phone\"},{\"Id\":\"screen-saver\",\"Type\":2,\"Accels\":["
            "\"XF86ScreenSaver\"],\"Name\":\"screen-saver\"},{\"Id\":\"a\",\"Type\":1,\"Accels\":["
            "\"F3\"],\"Name\":\"a\",\"Exec\":\"dde-control-center\"},{\"Id\":\"wm-switcher\","
            "\"Type\":0,\"Accels\":[\"<Shift><Super>Tab\"],\"Name\":\"切换窗口特效\"},{\"Id\":"
            "\"calculator\",\"Type\":2,\"Accels\":[\"XF86Calculator\"],\"Name\":\"Calculator\"},{"
            "\"Id\":\"audio-mic-mute\",\"Type\":2,\"Accels\":[\"XF86AudioMicMute\"],\"Name\":"
            "\"AudioMicMute\"},{\"Id\":\"move-to-workspace-1\",\"Type\":3,\"Accels\":[\"<Shift><"
            "Super>exclam\"],\"Name\":\"Move to workspace "
            "1\"},{\"Id\":\"show-desktop\",\"Type\":3,\"Accels\":[\"<Super>D\"],\"Name\":"
            "\"显示桌面\"},{\"Id\":\"switch-to-workspace-right\",\"Type\":3,\"Accels\":[\"<Super>"
            "Right\"],\"Name\":\"切换到右边工作区\"},{\"Id\":\"notification-center\",\"Type\":0,"
            "\"Accels\":[\"<Super>M\"],\"Name\":\"notification-center\"},{\"Id\":\"audio-lower-"
            "volume\",\"Type\":2,\"Accels\":[\"XF86AudioLowerVolume\"],\"Name\":"
            "\"AudioLowerVolume\"},{\"Id\":\"upper-layer-wlan\",\"Type\":2,\"Accels\":[],\"Name\":"
            "\"upper-layer-wlan\"},{\"Id\":\"messenger\",\"Type\":2,\"Accels\":[\"XF86Messenger\"],"
            "\"Name\":\"Messenger\"},{\"Id\":\"close\",\"Type\":3,\"Accels\":[\"<Alt>F4\"],"
            "\"Name\":\"关闭窗口\"},{\"Id\":\"move-to-workspace-2\",\"Type\":3,\"Accels\":[\"<"
            "Shift><Super>at\"],\"Name\":\"Move to workspace "
            "2\"},{\"Id\":\"switch-applications\",\"Type\":3,\"Accels\":[\"<Alt>Tab\"],\"Name\":"
            "\"切换窗口\"},{\"Id\":\"audio-raise-volume\",\"Type\":2,\"Accels\":["
            "\"XF86AudioRaiseVolume\"],\"Name\":\"AudioRaiseVolume\"},{\"Id\":\"paste\",\"Type\":2,"
            "\"Accels\":[\"XF86Paste\"],\"Name\":\"Paste\"},{\"Id\":\"move-to-workspace-7\","
            "\"Type\":3,\"Accels\":[],\"Name\":\"Move to workspace "
            "7\"},{\"Id\":\"terminal-quake\",\"Type\":0,\"Accels\":[\"<Alt>F2\"],\"Name\":"
            "\"终端雷神模式\"},{\"Id\":\"dos\",\"Type\":2,\"Accels\":[\"XF86DOS\"],\"Name\":"
            "\"dos\"},{\"Id\":\"toggle-fullscreen\",\"Type\":3,\"Accels\":[],\"Name\":\"toggle-"
            "fullscreen\"},{\"Id\":\"clipboard\",\"Type\":0,\"Accels\":[\"<Control><Alt>V\"],"
            "\"Name\":\"剪贴板\"},{\"Id\":\"audio-record\",\"Type\":2,\"Accels\":["
            "\"XF86AudioRecord\"],\"Name\":\"AudioRecord\"},{\"Id\":\"expose-windows\",\"Type\":3,"
            "\"Accels\":[\"<Super>W\"],\"Name\":\"显示当前工作区的窗口\"},{\"Id\":\"screenshot-"
            "fullscreen\",\"Type\":0,\"Accels\":[\"Print\"],\"Name\":\"全屏截图\"},{\"Id\":\"home-"
            "page\",\"Type\":2,\"Accels\":[\"XF86HomePage\"],\"Name\":\"HomePage\"},{\"Id\":"
            "\"toggle-above\",\"Type\":3,\"Accels\":[],\"Name\":\"Toggle window always appearing "
            "on "
            "top\"},{\"Id\":\"switch-to-workspace-12\",\"Type\":3,\"Accels\":[],\"Name\":\"Switch "
            "to workspace "
            "12\"},{\"Id\":\"switch-to-workspace-9\",\"Type\":3,\"Accels\":[\"<Super>9\"],\"Name\":"
            "\"Switch to workspace "
            "9\"},{\"Id\":\"screenshot-window\",\"Type\":0,\"Accels\":[\"<Alt>Print\"],\"Name\":"
            "\"窗口截图\"},{\"Id\":\"power-off\",\"Type\":2,\"Accels\":[\"XF86PowerOff\"],\"Name\":"
            "\"PowerOff\"},{\"Id\":\"kbd-light-on-off\",\"Type\":2,\"Accels\":["
            "\"XF86KbdLightOnOff\"],\"Name\":\"kbd-light-on-off\"},{\"Id\":\"audio-next\",\"Type\":"
            "2,\"Accels\":[\"XF86AudioNext\"],\"Name\":\"AudioNext\"},{\"Id\":\"wake-up\",\"Type\":"
            "2,\"Accels\":[\"XF86WakeUp\"],\"Name\":\"WakeUp\"},{\"Id\":\"switch-to-workspace-10\","
            "\"Type\":3,\"Accels\":[],\"Name\":\"Switch to workspace "
            "10\"},{\"Id\":\"system-monitor\",\"Type\":0,\"Accels\":[\"<Control><Alt>Escape\"],"
            "\"Name\":\"系统监视器\"},{\"Id\":\"maximize\",\"Type\":3,\"Accels\":[\"<Super>Up\"],"
            "\"Name\":\"最大化窗口\"},{\"Id\":\"move-to-workspace-5\",\"Type\":3,\"Accels\":[],"
            "\"Name\":\"Move to workspace "
            "5\"},{\"Id\":\"kbd-brightness-down\",\"Type\":2,\"Accels\":[\"XF86KbdBrightnessDown\"]"
            ",\"Name\":\"kbd-brightness-down\"},{\"Id\":\"ariplane-mode-toggle\",\"Type\":2,"
            "\"Accels\":[\"XF86RFKill\"],\"Name\":\"Airplane "
            "Mode\"},{\"Id\":\"minimize\",\"Type\":3,\"Accels\":[\"<Super>N\"],\"Name\":"
            "\"最小化窗口\"},{\"Id\":\"switch-group-backward\",\"Type\":3,\"Accels\":[\"<Shift><"
            "Alt>asciitilde\"],\"Name\":\"反向切换同类型窗口\"},{\"Id\":\"switch-to-workspace-3\","
            "\"Type\":3,\"Accels\":[\"<Super>3\"],\"Name\":\"Switch to workspace "
            "3\"},{\"Id\":\"mon-brightness-down\",\"Type\":2,\"Accels\":[\"XF86MonBrightnessDown\"]"
            ",\"Name\":\"MonBrightnessDown\"},{\"Id\":\"www\",\"Type\":2,\"Accels\":[\"XF86WWW\"],"
            "\"Name\":\"WWW\"},{\"Id\":\"move-to-workspace-10\",\"Type\":3,\"Accels\":[],\"Name\":"
            "\"Move to workspace "
            "10\"},{\"Id\":\"mail\",\"Type\":2,\"Accels\":[\"XF86Mail\"],\"Name\":\"Mail\"},{"
            "\"Id\":\"numlock\",\"Type\":2,\"Accels\":[\"Num_Lock\"],\"Name\":\"numlock\"},{\"Id\":"
            "\"maximize-vertically\",\"Type\":3,\"Accels\":[],\"Name\":\"Maximize window "
            "vertically\"},{\"Id\":\"switch-to-workspace-2\",\"Type\":3,\"Accels\":[\"<Super>2\"],"
            "\"Name\":\"Switch to workspace "
            "2\"},{\"Id\":\"ai-assistant\",\"Type\":0,\"Accels\":[\"<Super>Q\"],\"Name\":"
            "\"桌面智能助手\"},{\"Id\":\"scroll-up\",\"Type\":2,\"Accels\":[\"XF86ScrollUp\"],"
            "\"Name\":\"scroll-up\"},{\"Id\":\"back\",\"Type\":2,\"Accels\":[\"XF86Back\"],"
            "\"Name\":\"back\"},{\"Id\":\"search\",\"Type\":2,\"Accels\":[\"XF86Search\"],\"Name\":"
            "\"Search\"},{\"Id\":\"audio-prev\",\"Type\":2,\"Accels\":[\"XF86AudioPrev\"],\"Name\":"
            "\"AudioPrev\"},{\"Id\":\"mon-brightness-up\",\"Type\":2,\"Accels\":["
            "\"XF86MonBrightnessUp\"],\"Name\":\"MonBrightnessUp\"},{\"Id\":\"deepin-screen-"
            "recorder\",\"Type\":0,\"Accels\":[\"<Control><Alt>R\"],\"Name\":\"录屏\"},{\"Id\":"
            "\"disable-touchpad\",\"Type\":0,\"Accels\":[],\"Name\":\"禁用触控板\"},{\"Id\":"
            "\"speech-to-text\",\"Type\":0,\"Accels\":[\"<Control><Alt>O\"],\"Name\":\"语音听写\"},"
            "{\"Id\":\"touchpad-toggle\",\"Type\":2,\"Accels\":[\"XF86TouchpadToggle\"],\"Name\":"
            "\"ToggleTouchpad\"},{\"Id\":\"finance\",\"Type\":2,\"Accels\":[\"XF86Finance\"],"
            "\"Name\":\"finance\"},{\"Id\":\"send\",\"Type\":2,\"Accels\":[\"XF86Send\"],\"Name\":"
            "\"Send\"},{\"Id\":\"tools\",\"Type\":2,\"Accels\":[\"XF86Tools\"],\"Name\":\"Tools\"},"
            "{\"Id\":\"reply\",\"Type\":2,\"Accels\":[\"XF86Reply\"],\"Name\":\"Reply\"},{\"Id\":"
            "\"begin-resize\",\"Type\":3,\"Accels\":[\"<Alt>F8\"],\"Name\":\"改变窗口大小\"},{"
            "\"Id\":\"switch-to-workspace-11\",\"Type\":3,\"Accels\":[],\"Name\":\"Switch to "
            "workspace "
            "11\"},{\"Id\":\"turn-off-screen\",\"Type\":0,\"Accels\":[\"<Shift><Super>L\"],"
            "\"Name\":\"快速黑屏\"},{\"Id\":\"audio-play\",\"Type\":2,\"Accels\":["
            "\"XF86AudioPlay\"],\"Name\":\"AudioPlay\"},{\"Id\":\"activate-window-menu\",\"Type\":"
            "3,\"Accels\":[],\"Name\":\"Activate window "
            "menu\"},{\"Id\":\"toggle-maximized\",\"Type\":3,\"Accels\":[\"<Alt>F10\"],\"Name\":"
            "\"Toggle maximization "
            "state\"},{\"Id\":\"lock-screen\",\"Type\":0,\"Accels\":[\"<Super>L\"],\"Name\":"
            "\"锁屏界面\"},{\"Id\":\"menu-kb\",\"Type\":2,\"Accels\":[\"XF86MenuKB\"],\"Name\":"
            "\"menu-kb\"},{\"Id\":\"web-cam\",\"Type\":2,\"Accels\":[\"XF86WebCam\"],\"Name\":"
            "\"Camera\"},{\"Id\":\"audio-rewind\",\"Type\":2,\"Accels\":[\"XF86AudioRewind\"],"
            "\"Name\":\"AudioRewind\"},{\"Id\":\"touchpad-on\",\"Type\":2,\"Accels\":["
            "\"XF86TouchpadOn\"],\"Name\":\"touchpad-on\"},{\"Id\":\"move-to-workspace-4\","
            "\"Type\":3,\"Accels\":[\"<Shift><Super>dollar\"],\"Name\":\"Move to workspace "
            "4\"},{\"Id\":\"move-to-workspace-8\",\"Type\":3,\"Accels\":[],\"Name\":\"Move to "
            "workspace "
            "8\"},{\"Id\":\"switch-to-workspace-5\",\"Type\":3,\"Accels\":[\"<Super>5\"],\"Name\":"
            "\"Switch to workspace "
            "5\"},{\"Id\":\"screenshot\",\"Type\":0,\"Accels\":[\"<Control><Alt>A\"],\"Name\":"
            "\"截图\"},{\"Id\":\"text-to-speech\",\"Type\":0,\"Accels\":[\"<Control><Alt>P\"],"
            "\"Name\":\"语音朗读\"},{\"Id\":\"audio-media\",\"Type\":2,\"Accels\":["
            "\"XF86AudioMedia\"],\"Name\":\"AudioMedia\"},{\"Id\":\"audio-forward\",\"Type\":2,"
            "\"Accels\":[\"XF86AudioForward\"],\"Name\":\"audio-forward\"},{\"Id\":\"go\",\"Type\":"
            "2,\"Accels\":[\"XF86Go\"],\"Name\":\"go\"},{\"Id\":\"task-pane\",\"Type\":2,"
            "\"Accels\":[\"XF86TaskPane\"],\"Name\":\"task-pane\"},{\"Id\":\"new\",\"Type\":2,"
            "\"Accels\":[\"XF86New\"],\"Name\":\"New\"},{\"Id\":\"move-to-workspace-12\",\"Type\":"
            "3,\"Accels\":[],\"Name\":\"Move to workspace "
            "12\"},{\"Id\":\"switch-applications-backward\",\"Type\":3,\"Accels\":[\"<Shift><Alt>"
            "Tab\"],\"Name\":\"反向切换窗口\"},{\"Id\":\"switch-to-workspace-1\",\"Type\":3,"
            "\"Accels\":[\"<Super>1\"],\"Name\":\"Switch to workspace "
            "1\"},{\"Id\":\"explorer\",\"Type\":2,\"Accels\":[\"XF86Explorer\"],\"Name\":"
            "\"Explorer\"},{\"Id\":\"battery\",\"Type\":2,\"Accels\":[\"XF86Battery\"],\"Name\":"
            "\"battery\"},{\"Id\":\"documents\",\"Type\":2,\"Accels\":[\"XF86Documents\"],\"Name\":"
            "\"Documents\"},{\"Id\":\"move-to-workspace-3\",\"Type\":3,\"Accels\":[\"<Shift><Super>"
            "numbersign\"],\"Name\":\"Move to workspace 3\"}]");
    QString key;
    widget->prepareSearchKeys();
    QLineEdit *m_searchInput = widget->findChild<QLineEdit *>("KeyboardLineEdit");
    m_searchInput->textChanged("l");
    m_searchInput->textChanged("");
    widget->onRemoveItem("app-store", 1);
    EXPECT_NO_THROW(model->setCurrentInfo(model->getInfo("<Control><Alt>T")));
    model->keyEvent(true, "<Control><Alt>T");
    model->onCustomInfo("{\"Id\":\"printer\",\"Type\":1,\"Accels\":[\"<Control>P\"],\"Name\":"
                        "\"printer\",\"Exec\":\"/usr/bin/dde-printer\"}\n");
    model->onParseInfo("[{\"Id\":\"lock-screen\",\"Type\":0,\"Accels\":[\"<Super>L\"],\"Name\":"
                       "\"锁屏界面\"}]");
    widget->onSearchInfo(model->getInfo("<Control><Alt>T"), key);
    widget->onResetFinished();
    EXPECT_NO_THROW(model->setSearchResult(str));
}
