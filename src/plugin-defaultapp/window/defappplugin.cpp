#include "defappdetailwidget.h"
#include "defappplugin.h"
#include "defappmodel.h"
#include "defappworker.h"
#include "addbuttonwidget.h"
#include "widgets/widgetmodule.h"

#include <QApplication>

#include <DFloatingButton>
#include <DStyle>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

struct DATE {
    QString name;
    QString displayName;
    QString icon;
    DefAppWorker::DefaultAppsCategory category;
    DATE(const QString &_name, const QString &_displayName, const QString _icon, DefAppWorker::DefaultAppsCategory _category)
        : name(_name)
        , displayName(_displayName)
        , icon(_icon)
        , category(_category)
    {}
};

QString DefAppPlugin::name() const
{
    return QStringLiteral("Default Applications");
}

ModuleObject *DefAppPlugin::module()
{
    QList<DATE> moduleInfo = {
        DATE( "defappWebpage",tr("Webpage"), "dcc_browser", DefAppWorker::DefaultAppsCategory::Browser),
        DATE("defappMail",tr("Mail"), "dcc_mail", DefAppWorker::DefaultAppsCategory::Mail),
        DATE("defappText",tr("Text"), "dcc_text", DefAppWorker::DefaultAppsCategory::Text),
        DATE("defappMusic",tr("Music"),"dcc_music", DefAppWorker::DefaultAppsCategory::Music),
        DATE("defappVideo",tr("Video"),"dcc_video", DefAppWorker::DefaultAppsCategory::Video),
        DATE("defappPicture",tr("Picture"),"dcc_photo", DefAppWorker::DefaultAppsCategory::Picture),
        DATE("defappTerminal",tr("Terminal"), "dcc_terminal", DefAppWorker::DefaultAppsCategory::Terminal),
    };
    // 一级页面
    DefAppModule *moduleRoot = new DefAppModule;
    moduleRoot->setChildType(ModuleObject::VList);

    for (DATE iter : moduleInfo) {
        // 二级按钮页
        DefAppsButtonModule *moduleDefaultApps = new DefAppsButtonModule(iter.category, iter.name, iter.displayName,
                                                                         QIcon::fromTheme(QString(iter.icon)),
                                                                         moduleRoot->model(), moduleRoot->work());

        moduleDefaultApps->setChildType(ModuleObject::Page);

        // 三级页面
        DefappDetailModule *defappDetail = new DefappDetailModule(iter.category, moduleRoot->model(), moduleRoot->work());
        defappDetail->setChildType(ModuleObject::Page);
        moduleDefaultApps->appendChild(defappDetail);

        ModuleObject *addButton = new WidgetModule<AddButtonWidget>("defappApplistAddbtn","addDefApp",[iter,moduleRoot](AddButtonWidget *button){
            button->setDefaultAppsCategory(iter.category);
            button->setModel(moduleRoot->model());
            connect(button, &AddButtonWidget::requestCreateFile, moduleRoot->work(), &DefAppWorker::onCreateFile);
        });
        addButton->setExtra();
        moduleDefaultApps->appendChild(addButton);

        connect(moduleDefaultApps, &DefAppsButtonModule::onButtonClicked, moduleDefaultApps, [moduleDefaultApps] {
            moduleDefaultApps->children(0)->trigger();
        });
        moduleRoot->appendChild(moduleDefaultApps);
    }
    return moduleRoot;
}

int DefAppPlugin::location() const
{
    return 4;
}

DefAppModule::DefAppModule(QObject *parent)
    : ModuleObject("defapp", tr("Default Applications"), tr("Default Applications"), QIcon::fromTheme("dcc_nav_defapp"), parent)
    , m_model(new DefAppModel(this))
    , m_work(new DefAppWorker(m_model, this))
    , m_defApps(nullptr)
{
}

DefAppModule::~DefAppModule() {
    m_model->deleteLater();
    m_work->deleteLater();
}

void DefAppModule::active()
{
    m_work->onGetListApps();
}

DefAppsButtonModule::DefAppsButtonModule(DefAppWorker::DefaultAppsCategory category, const QString &name, const QString &displayName, const QIcon &icon,  DefAppModel *model, DefAppWorker *work)
    : ModuleObject(name, displayName, icon)
    , m_category(category)
    , m_model(model)
    , m_work(work)
{

}

DefAppsButtonModule::~DefAppsButtonModule()
{
}

QWidget *DefAppsButtonModule::page(){
    DefappDetailWidget *defDetail = new DefappDetailWidget(m_category);
    defDetail->setModel(m_model);

    return defDetail;
}

// 三级页面
DefappDetailModule::DefappDetailModule(DefAppWorker::DefaultAppsCategory category, DefAppModel *model, DefAppWorker *work)
    : ModuleObject("defappApplistDefapp")
{
    m_category = category;
    m_model = model;
    m_work = work;
}

QWidget *DefappDetailModule::page()
{
    DefappDetailWidget *defDetail = new DefappDetailWidget(m_category);
    defDetail->setModel(m_model);
    //设置默认程序
    connect(defDetail, &DefappDetailWidget::requestSetDefaultApp, m_work, &DefAppWorker::onSetDefaultApp);
    connect(defDetail, &DefappDetailWidget::requestDelUserApp, m_work, &DefAppWorker::onDelUserApp);
    return defDetail;
}

