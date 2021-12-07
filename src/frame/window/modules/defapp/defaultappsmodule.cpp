/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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

#include "defaultappsmodule.h"
#include "defappwidget.h"
#include "defappdetailwidget.h"
#include "modules/defapp/defappmodel.h"
#include "modules/defapp/defappworker.h"
#include "window/mainwindow.h"
#include "window/gsettingwatcher.h"

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::defapp;

DefaultAppsModule::DefaultAppsModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
{
    m_pMainWindow = dynamic_cast<MainWindow *>(m_frameProxy);
    GSettingWatcher::instance()->insertState("defappWebpage");
    GSettingWatcher::instance()->insertState("defappMail");
    GSettingWatcher::instance()->insertState("defappMusic");
    GSettingWatcher::instance()->insertState("defappVideo");
    GSettingWatcher::instance()->insertState("defappPicture");
    GSettingWatcher::instance()->insertState("defappTerminal");

    //~ contents_path /defapp/Text/Add Application
    //~ child_page Text
    tr("Add Application");
}

DefaultAppsModule::~DefaultAppsModule()
{
    if (m_defAppModel)
        m_defAppModel->deleteLater();
    m_defAppModel = nullptr;

    if (m_defAppWorker)
        m_defAppWorker->deleteLater();
    m_defAppWorker = nullptr;
}

void DefaultAppsModule::preInitialize(bool sync, FrameProxyInterface::PushType)
{
    Q_UNUSED(sync)

    if (m_defAppModel) {
        delete m_defAppModel;
    }
    m_defAppModel  = new dcc::defapp::DefAppModel;
    m_defAppWorker = new dcc::defapp::DefAppWorker(m_defAppModel);
    m_defAppModel->moveToThread(qApp->thread());
    m_defAppWorker->moveToThread(qApp->thread());
    m_defAppWorker->onGetListApps();
\
    addChildPageTrans();
    initSearchData();
}

void DefaultAppsModule::initialize()
{

}

void DefaultAppsModule::active()
{
    DefaultAppsWidget* defaultappsWidget = new DefaultAppsWidget;
    defaultappsWidget->setVisible(false);
    connect(this, &DefaultAppsModule::requestSetDefappCategory, defaultappsWidget, &DefaultAppsWidget::setCurrentIndex);
    connect(defaultappsWidget, &DefaultAppsWidget::requestCategoryClicked, this, &DefaultAppsModule::showDetailWidget);
    m_frameProxy->pushWidget(this, defaultappsWidget);
    defaultappsWidget->setVisible(true);
    connect(defaultappsWidget, &DefaultAppsWidget::requestUpdateSecondMenu, this, [=] (bool needPop) {
           if (m_pMainWindow->getcontentStack().size() >= 2 && needPop)
               m_frameProxy->popWidget(this);
           defaultappsWidget->showDefaultWidget();
       });

    defaultappsWidget->showDefaultWidget();
}

const QString DefaultAppsModule::name() const
{
    return QStringLiteral("defapp");
}

const QString DefaultAppsModule::displayName() const
{
    return tr("Default Applications");
}

void DefaultAppsModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}

int DefaultAppsModule::load(const QString &path)
{
    QMap<QString, dcc::defapp::DefAppWorker::DefaultAppsCategory> maps = {
        { QStringLiteral("Webpage"), dcc::defapp::DefAppWorker::Browser},
        { QStringLiteral("Mail"), dcc::defapp::DefAppWorker::Mail},
        { QStringLiteral("Text"), dcc::defapp::DefAppWorker::Text},
        { QStringLiteral("Music"), dcc::defapp::DefAppWorker::Music},
        { QStringLiteral("Video"), dcc::defapp::DefAppWorker::Video},
        { QStringLiteral("Picture"), dcc::defapp::DefAppWorker::Picture},
        { QStringLiteral("Terminal"), dcc::defapp::DefAppWorker::Terminal},
    };
    QStringList pathList = path.split("/");
    int count = pathList.count();
    if (count <= 0) {
        return -1;
    }
    QString loadPath = path.split("/").at(0);
    dcc::defapp::DefAppWorker::DefaultAppsCategory currentCategory = maps.value(loadPath, dcc::defapp::DefAppWorker::Browser);
    showDetailWidget(currentCategory);
    Q_EMIT requestSetDefappCategory(currentCategory);

    if (count == 2 && pathList.at(1) == "Add Application") {
        Q_EMIT requestAddDefault();
    }

    return 0;
}

QStringList DefaultAppsModule::availPage() const
{
    return {
        "Webpage"
        , "Mail"
        , "Text"
        , "Music"
        , "Video"
        , "Picture"
        , "Terminal"
    };
}

void DefaultAppsModule::addChildPageTrans() const
{
    if (m_frameProxy != nullptr) {
        //defapp
        m_frameProxy->addChildPageTrans("Webpage", tr("Webpage"));
        m_frameProxy->addChildPageTrans("Mail", tr("Mail"));
        m_frameProxy->addChildPageTrans("Text", tr("Text"));
        m_frameProxy->addChildPageTrans("Music", tr("Music"));
        m_frameProxy->addChildPageTrans("Video", tr("Video"));
        m_frameProxy->addChildPageTrans("Picture", tr("Picture"));
        m_frameProxy->addChildPageTrans("Terminal", tr("Terminal"));
    }
}

void DefaultAppsModule::initSearchData()
{
    const QString& module = displayName();
    const QString& addApplication = tr("Add Application");

    static QMap<QString, bool> gsettingsMap;

    auto func_is_visible = [=](const QString &gsettings) {
        bool ret = GSettingWatcher::instance()->get(gsettings).toBool();
        gsettingsMap.insert(gsettings, ret);
        return ret;
    };

    auto func_second_changed = [ = ]() {
        m_frameProxy->setWidgetVisible(module, tr("Webpage"), func_is_visible("defappWebpage"));
        m_frameProxy->setWidgetVisible(module, tr("Mail"), func_is_visible("defappMail"));
        m_frameProxy->setWidgetVisible(module, tr("Text"), func_is_visible("defappText"));
        m_frameProxy->setWidgetVisible(module, tr("Music"), func_is_visible("defappMusic"));
        m_frameProxy->setWidgetVisible(module, tr("Video"), func_is_visible("defappVideo"));
        m_frameProxy->setWidgetVisible(module, tr("Picture"), func_is_visible("defappPicture"));
        m_frameProxy->setWidgetVisible(module, tr("Terminal"), func_is_visible("defappTerminal"));
    };

    auto func_addapp_changed = [ = ]() {
        bool bAddApplication = GSettingWatcher::instance()->get("defappApplistAddbtn") != "Hidden";
        gsettingsMap.insert("defappApplistAddbtn", bAddApplication);

        func_second_changed();

        m_frameProxy->setDetailVisible(module, tr("Webpage"), addApplication, func_is_visible("defappWebpage") && bAddApplication);
        m_frameProxy->setDetailVisible(module, tr("Mail"), addApplication, func_is_visible("defappMail") && bAddApplication);
        m_frameProxy->setDetailVisible(module, tr("Text"), addApplication, func_is_visible("defappText") && bAddApplication);
        m_frameProxy->setDetailVisible(module, tr("Music"), addApplication, func_is_visible("defappMusic") && bAddApplication);
        m_frameProxy->setDetailVisible(module, tr("Video"), addApplication, func_is_visible("defappVideo") && bAddApplication);
        m_frameProxy->setDetailVisible(module, tr("Picture"), addApplication, func_is_visible("defappPicture") && bAddApplication);
        m_frameProxy->setDetailVisible(module, tr("Terminal"), addApplication, func_is_visible("defappTerminal") && bAddApplication);
    };

    auto func_process_all = [ = ]() {
        func_second_changed();

        func_addapp_changed();
    };

    connect(GSettingWatcher::instance(), &GSettingWatcher::notifyGSettingsChanged, this, [=](const QString &gsetting, const QString &state) {
        Q_UNUSED(state);
        if (!gsettingsMap.contains(gsetting)) {
            return;
        }
        const bool status = GSettingWatcher::instance()->get(gsetting).toBool();
        if (gsettingsMap.value(gsetting) == status) {
            return;
        }

        if ("defappWebpage" == gsetting) {
            m_frameProxy->setWidgetVisible(module, tr("Webpage"), func_is_visible("defappWebpage"));
        } else if ("defappText" == gsetting) {
            m_frameProxy->setWidgetVisible(module, tr("Text"), func_is_visible("defappText"));
        } else if ("defappMusic" == gsetting) {
            m_frameProxy->setWidgetVisible(module, tr("Music"), func_is_visible("defappMusic"));
        } else if ("defappMail" == gsetting) {
            m_frameProxy->setWidgetVisible(module, tr("Mail"), func_is_visible("defappMail"));
        } else if ("defappVideo" == gsetting) {
            m_frameProxy->setWidgetVisible(module, tr("Video"), func_is_visible("defappVideo"));
        } else if ("defappPicture" == gsetting) {
            m_frameProxy->setWidgetVisible(module, tr("Picture"), func_is_visible("defappPicture"));
        } else if ("defappTerminal" == gsetting) {
            m_frameProxy->setWidgetVisible(module, tr("Terminal"), func_is_visible("defappTerminal"));
        } else if ("defappApplistAddbtn" == gsetting) {
            func_addapp_changed();
        } else {
            qWarning() << " not contains the gsettings : " << gsetting << status;
            return;
        }

        qInfo() << " [notifyGSettingsChanged]  gsetting, state :" << gsetting << status;
        m_frameProxy->updateSearchData(module);
    });

    func_process_all();
}

void DefaultAppsModule::showDetailWidget(dcc::defapp::DefAppWorker::DefaultAppsCategory category) {
    DefappDetailWidget* detailWidget = new DefappDetailWidget(category);
    detailWidget->setVisible(false);
    detailWidget->setModel(m_defAppModel);
    connect(detailWidget, &DefappDetailWidget::requestSetDefaultApp, m_defAppWorker, &dcc::defapp::DefAppWorker::onSetDefaultApp); //设置默认程序
    connect(detailWidget, &DefappDetailWidget::requestDelUserApp, m_defAppWorker, &dcc::defapp::DefAppWorker::onDelUserApp);
    connect(detailWidget, &DefappDetailWidget::requestCreateFile, m_defAppWorker, &dcc::defapp::DefAppWorker::onCreateFile);
    connect(this, &DefaultAppsModule::requestAddDefault, detailWidget, &DefappDetailWidget::onAddBtnClicked);
    m_frameProxy->pushWidget(this, detailWidget);
    detailWidget->setVisible(true);
}
