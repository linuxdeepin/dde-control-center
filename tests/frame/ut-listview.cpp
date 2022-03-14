#include "gtest/gtest.h"

#include "src/frame/mainwindow.h"
#include "src/frame/listitemdelegate.h"
#include "src/frame/listview.h"
#include "src/frame/moduledatamodel.h"
#include "interface/moduleobject.h"

DCC_USE_NAMESPACE
const static QString G_icons[] = {
    "chmsee-icon",
    "deepin-compressor",
    "deepin-music",
    "fcitx",
    "preferences-desktop-font",
    "com.deepin.gomoku",
    "deepin-contacts",
    "deepin-note",
    "firefox",
    "preferences-system",
    "com.deepin.lianliankan",
    "deepin-deb-installer",
    "deepin-phone-assistant",
    "foxitreader",
    "scanner",
    "com.qq.weixin.deepin",
    "deepin-defender",
    "deepin-picker",
    "gedit",
    "steam",
    "crossover",
    "deepin-devicemanager",
    "deepin-reader",
    "gimp",
    "thunderbird",
    "dde-calendar",
    "deepin-diskmanager",
    "deepin-remote-assistance",
    "gnome-mime-application-x-deb",
    "uosbrowser-cn",
    "dde-file-manager",
    "deepin-draw",
    "deepin-repair-tools",
    "google-chrome",
    "uos-downloadmanager",
    "dde-introduction",
    "deepin-editor",
    "deepin-scanner",
    "gparted",
    "uos-installtool",
    "dde-printer",
    "deepin-feedback",
    "deepin-screen-recorder",
    "iso-customizer",
    "uos-remote-assistance",
    "deepin-album",
    "deepin-font-manager",
    "deepin-system-monitor",
    "kbox",
    "uos-service-support",
    "deepin-app-store",
    "deepin-graphics-driver-manager",
    "deepin-terminal",
    "kmsclient",
    "uos-windesk",
    "deepin-boot-maker",
    "deepin-image-viewer",
    "deepin-toggle-desktop",
    "kmstools",
    "wechat",
    "deepin-browser",
    "deepin-installer",
    "deepin-voice-note",
    "licensequick",
    "wps-office2019-etmain",
    "deepin-calculator",
    "deepin-log-viewer",
    "deepin-voice-recorder",
    "netease-cloud-music",
    "wps-office2019-wppmain",
    "deepin-camera",
    "deepin-mail",
    "deepin-wine-assist",
    "onboard",
    "wps-office2019-wpsmain",
    "deepin-clone",
    "deepin-manual",
    "desktop-ai-assistant",
    "org.deepin.browser",
    "wps-office-etmain",
    "deepin-cloud-print-configurator",
    "deepin-movie",
    "evince",
    "org.deepin.contacts",
    "wps-office-wppmain",
    "deepin-cloud-scan-config-helper",
    "deepin-multitasking-view",
    "fcitx-setting",
    "org.gnome.Evince",
    "wps-office-wpsmain"
};

class Tst_ListView : public testing::Test
{
public:
    void SetUp() override
    {
        view = new ListView;
        delegate = new ListItemDelegate(view);
        view->setItemDelegate(delegate);
        model = new ModuleDataModel(view);
        view->setModel(model);
    }
    void TearDown() override
    {
        delete view;
        view = nullptr;
    }

public:
    ListView *view = nullptr;
    ModuleDataModel *model = nullptr;
    ListItemDelegate *delegate = nullptr;
};

TEST_F(Tst_ListView, coverage)
{
    // 按回车时触发
    view->connect(view, &ListView::activated, view, [](const QModelIndex &index) {
        qInfo() << __FILE__ << __LINE__ << "activated" << index;
    });
    view->connect(view, &ListView::clicked, view, [this](const QModelIndex &index) {
        qInfo() << __FILE__ << __LINE__ << "clicked" << index;
        QMargins margins = view->contentsMargins();
        switch (index.row()) {
        case 0: // 模式
            view->setViewMode(ListView::IconMode);
            break;
        case 1:
            view->setViewMode(ListView::ListMode);
            break;
        case 2: // 间距
            view->setSpacing(view->spacing() + 1);
            break;
        case 3:
            view->setSpacing(view->spacing() - 1);
            break;
        case 4: // 边框大小
            qInfo() << __FILE__ << __LINE__ << margins;
            margins.setTop(margins.top() + 1);
            qInfo() << __FILE__ << __LINE__ << margins;
            //            view->setContentsMargins(margins);
            view->setContentsMargins(8, 8, 8, 8);
            break;
        case 5:
            margins.setTop(margins.top() - 1);
            view->setContentsMargins(margins);
            break;
        case 6:
            margins.setLeft(margins.left() + 1);
            view->setContentsMargins(margins);
            break;
        case 7:
            margins.setLeft(margins.left() - 1);
            view->setContentsMargins(margins);
            break;
        case 8:
            margins.setBottom(margins.bottom() + 1);
            view->setContentsMargins(margins);
            break;
        case 9:
            margins.setBottom(margins.bottom() - 1);
            view->setContentsMargins(margins);
            break;
        case 10:
            margins.setRight(margins.right() + 1);
            view->setContentsMargins(margins);
            break;
        case 11:
            margins.setRight(margins.right() - 1);
            view->setContentsMargins(margins);
            break;
        }
        qInfo() << __FILE__ << __LINE__ << view->contentsMargins();
    });
    view->connect(view, &ListView::doubleClicked, view, [](const QModelIndex &index) {
        qInfo() << __FILE__ << __LINE__ << "doubleClicked" << index;
    });
    view->connect(view, &ListView::entered, view, [](const QModelIndex &index) {
        qInfo() << __FILE__ << __LINE__ << "entered" << index;
    });
    view->connect(view, &ListView::iconSizeChanged, view, [](const QSize &size) {
        qInfo() << __FILE__ << __LINE__ << "iconSizeChanged" << size;
    });
    view->connect(view, &ListView::pressed, view, [this](const QModelIndex &index) {
        qInfo() << __FILE__ << __LINE__ << "pressed" << index << view->currentIndex();
    });
    view->connect(view, &ListView::viewportEntered, view, []() {
        qInfo() << __FILE__ << __LINE__ << "viewportEntered";
    });
    //　选中顶改变时触发
    view->connect(view->selectionModel(), &QItemSelectionModel::selectionChanged, view, [](const QItemSelection &selected, const QItemSelection &deselected) {
        qInfo() << __FILE__ << __LINE__ << "selectionChanged" << selected << deselected;
    });
    view->connect(view->selectionModel(), &QItemSelectionModel::currentChanged, view, [](const QModelIndex &current, const QModelIndex &previous) {
        qInfo() << __FILE__ << __LINE__ << "currentChanged" << current << previous;
    });

    ModuleObject *module = new ModuleObject();
    for (int i = 0; i < 17; i++) {
        ModuleObject *modulechild = new ModuleObject();
        ModuleData *data = new ModuleData;
        data->Name = QString("firstmenu%1").arg(i);
        data->DisplayName = QString("主菜单%1").arg(i);
        data->Description = QString("我是主菜单%1").arg(i);
        data->Icon = QIcon::fromTheme(G_icons[i]);
        modulechild->setModuleData(data);
        module->appendChild(modulechild);
    }
    model->setData(module);
}
