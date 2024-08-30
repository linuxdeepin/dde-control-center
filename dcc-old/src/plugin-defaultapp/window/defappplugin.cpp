// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "defappplugin.h"

#include "addbuttonwidget.h"
#include "defappdetailwidget.h"
#include "defappmodel.h"
#include "defappworker.h"
#include "interface/pagemodule.h"
#include "mimedbusproxyold.h"
#include "widgets/itemmodule.h"

#include <DFloatingButton>
#include <DIconTheme>
#include <DStyle>

#include <QApplication>
#include <QVBoxLayout>

DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

struct DATE
{
    QString name;
    QString displayName;
    QString icon;
    DefAppWorker::DefaultAppsCategory category;

    DATE(const QString &_name,
         const QString &_displayName,
         const QString &_icon,
         DefAppWorker::DefaultAppsCategory _category)
        : name(_name)
        , displayName(_displayName)
        , icon(_icon)
        , category(_category)
    {
    }
};

QString DefAppPlugin::name() const
{
    return QStringLiteral("Default Applications");
}

ModuleObject *DefAppPlugin::module()
{
    QList<DATE> moduleInfo = {
        DATE("defappWebpage",
             tr("Webpage"),
             "dcc_browser",
             DefAppWorker::DefaultAppsCategory::Browser),
        DATE("defappMail", tr("Mail"), "dcc_mail", DefAppWorker::DefaultAppsCategory::Mail),
        DATE("defappText", tr("Text"), "dcc_text", DefAppWorker::DefaultAppsCategory::Text),
        DATE("defappMusic", tr("Music"), "dcc_music", DefAppWorker::DefaultAppsCategory::Music),
        DATE("defappVideo", tr("Video"), "dcc_video", DefAppWorker::DefaultAppsCategory::Video),
        DATE("defappPicture",
             tr("Picture"),
             "dcc_photo",
             DefAppWorker::DefaultAppsCategory::Picture),
        DATE("defappTerminal",
             tr("Terminal"),
             "dcc_terminal",
             DefAppWorker::DefaultAppsCategory::Terminal),
    };
    // 一级页面
    DefAppModule *moduleRoot = new DefAppModule;

    for (DATE iter : moduleInfo) {
        PageModule *defappDetail = new PageModule(iter.name,
                                                  iter.displayName,
                                                  QVariant::fromValue(iter.icon),
                                                  moduleRoot);

        defappDetail->appendChild(new ItemModule(
                "",
                "",
                [iter, moduleRoot]([[maybe_unused]] ModuleObject *module) {
                    DefappDetailWidget *defDetail = new DefappDetailWidget(iter.category);
                    defDetail->setDetailModel(moduleRoot->model());
                    // 设置默认程序
                    if (moduleRoot->isOldInterface()) {
                        connect(defDetail,
                                &DefappDetailWidget::requestSetDefaultApp,
                                moduleRoot->oldWork(),
                                &DefAppWorkerOld::onSetDefaultApp);
                        connect(defDetail,
                                &DefappDetailWidget::requestDelUserApp,
                                moduleRoot->oldWork(),
                                &DefAppWorkerOld::onDelUserApp);
                    } else {
                        connect(defDetail,
                                &DefappDetailWidget::requestSetDefaultApp,
                                moduleRoot->work(),
                                &DefAppWorker::onSetDefaultApp);
                        connect(defDetail,
                                &DefappDetailWidget::requestDelUserApp,
                                moduleRoot->work(),
                                &DefAppWorker::onDelUserApp);
                    }
                    return defDetail;
                },
                false));
        // TODO: if we need add again?
        // ModuleObject *addButton =
        //         new WidgetModule<AddButtonWidget>("defappApplistAddbtn",
        //                                           "addDefApp",
        //                                           [iter, moduleRoot](AddButtonWidget *button) {
        //                                               button->setDefaultAppsCategory(iter.category);
        //                                               button->setModel(moduleRoot->model());
        //                                               connect(button,
        //                                                       &AddButtonWidget::requestCreateFile,
        //                                                       moduleRoot->work(),
        //                                                       &DefAppWorker::onCreateFile);
        //                                           });
        // addButton->setExtra();
        // defappDetail->appendChild(addButton);
        moduleRoot->appendChild(defappDetail);
    }
    return moduleRoot;
}

QString DefAppPlugin::location() const
{
    return "4";
}

DefAppModule::DefAppModule(QObject *parent)
    : VListModule(
            "defapp", tr("Default Applications"), DIconTheme::findQIcon("dcc_nav_defapp"), parent)
    , m_model(new DefAppModel(this))
    , m_defApps(nullptr)
    , m_isOldInterface(false)
{
    if (MimeDBusProxyOld::isRegisted()) {
        m_oldwork = new DefAppWorkerOld(m_model, this);
        m_isOldInterface = true;
    } else {
        m_work = new DefAppWorker(m_model, this);
    }
}

DefAppModule::~DefAppModule()
{
    m_model->deleteLater();
    if (m_isOldInterface) {
        m_oldwork->deleteLater();
    } else {
        m_work->deleteLater();
    }
}

void DefAppModule::active()
{
    if (m_isOldInterface) {
        m_oldwork->onGetListApps();
    } else {
        m_work->onGetListApps();
    }
}
