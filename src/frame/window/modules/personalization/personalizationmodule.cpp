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
#include "personalizationmodule.h"
#include "personalizationlist.h"
#include "modules/personalization/personalizationmodel.h"
#include "modules/personalization/personalizationwork.h"
#include "personalizationgeneral.h"
#include "perssonalizationthemewidget.h"
#include "personalizationthemelist.h"
#include "personalizationfontswidget.h"
#include "window/mainwindow.h"
#include "window/gsettingwatcher.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::personalization;

PersonalizationModule::PersonalizationModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_model(nullptr)
    , m_work(nullptr)
{
    m_pMainWindow = dynamic_cast<MainWindow *>(m_frameProxy);
    GSettingWatcher::instance()->insertState("personalizationGeneral");
    GSettingWatcher::instance()->insertState("personalizationIconTheme");
    GSettingWatcher::instance()->insertState("personalizationCursorTheme");
    GSettingWatcher::instance()->insertState("personalizationFont");
}

PersonalizationModule::~PersonalizationModule()
{
    if (m_model)
        m_model->deleteLater();
    if (m_work)
        m_work->deleteLater();
}

void PersonalizationModule::preInitialize(bool sync, FrameProxyInterface::PushType)
{
    Q_UNUSED(sync)

    if (m_model) {
        delete m_model;
    }
    m_model  = new dcc::personalization::PersonalizationModel;
    m_work = new dcc::personalization::PersonalizationWork(m_model);

    addChildPageTrans();
    initSearchData();
}

void PersonalizationModule::initialize()
{
    m_model->moveToThread(qApp->thread());
    m_work->moveToThread(qApp->thread());
}

const QString PersonalizationModule::name() const
{
    return QStringLiteral("personalization");
}

const QString PersonalizationModule::displayName() const
{
    return tr("Personalization");
}

void PersonalizationModule::active()
{
    PersonalizationList *firstWidget = new PersonalizationList();
    firstWidget->setVisible(false);
    firstWidget->setAccessibleName("personanization");
    connect(firstWidget, &PersonalizationList::requestShowGeneral, this, &PersonalizationModule::showGenaralWidget);
    connect(firstWidget, &PersonalizationList::requestShowIconTheme, this, &PersonalizationModule::showIconThemeWidget);
    connect(firstWidget, &PersonalizationList::requestShowCursorTheme, this, &PersonalizationModule::showCursorThemeWidget);
    connect(firstWidget, &PersonalizationList::requestShowFonts, this, &PersonalizationModule::showFontThemeWidget);
    connect(this, &PersonalizationModule::requestSetCurrentIndex, firstWidget, &PersonalizationList::setCurrentIndex);
    m_frameProxy->pushWidget(this, firstWidget);
    firstWidget->setVisible(true);

    connect(firstWidget, &PersonalizationList::requestUpdateSecondMenu, this, [=] (bool needPop) {
        if (m_pMainWindow->getcontentStack().size() >= 2 && needPop)
            m_frameProxy->popWidget(this);
        firstWidget->showDefaultWidget();
    });

    firstWidget->showDefaultWidget();
}

void PersonalizationModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}

int PersonalizationModule::load(const QString &path)
{
    QString loadPath = path.split("/").at(0);
    int row = -1;
    if (loadPath == QStringLiteral("General")) {
        row = 0;
    } else if (loadPath == QStringLiteral("Icon Theme")) {
        row = 1;
    } else if (loadPath == QStringLiteral("Cursor Theme")) {
        row = 2;
    } else if (loadPath == QStringLiteral("Font")) {
        row = 3;
    } else {
        row = availPage().indexOf(loadPath);
    }
    Q_EMIT requestSetCurrentIndex(row);
    return row == -1 ? -1 : 0;
}

QStringList PersonalizationModule::availPage() const
{
    QStringList pages;
    pages << "General" << "Icon Theme" << "Cursor Theme" << "Font";
    pages.append(InsertPlugin::instance()->availPages("personalization"));
    return pages;
}

void PersonalizationModule::addChildPageTrans() const
{
    if (m_frameProxy != nullptr) {
        //personalization
        m_frameProxy->addChildPageTrans("Font", tr("Font"));
        m_frameProxy->addChildPageTrans("Icon Theme", tr("Icon Theme"));
        m_frameProxy->addChildPageTrans("Cursor Theme", tr("Cursor Theme"));
    }
}

void PersonalizationModule::showGenaralWidget()
{
    m_work->refreshTheme();

    PersonalizationGeneral *widget = new PersonalizationGeneral;
    widget->setVisible(false);
    widget->setAccessibleName("personalizationgeneral");

    widget->setModel(m_model);
    connect(widget->getThemeWidget(), &PerssonalizationThemeWidget::requestSetDefault, m_work, &dcc::personalization::PersonalizationWork::setDefault);
    connect(widget, &PersonalizationGeneral::requestSetOpacity, m_work, &dcc::personalization::PersonalizationWork::setOpacity);
    connect(widget, &PersonalizationGeneral::requestSetMiniEffect, m_work, &dcc::personalization::PersonalizationWork::setMiniEffect);
    connect(widget, &PersonalizationGeneral::requestWindowSwitchWM, m_work, &dcc::personalization::PersonalizationWork::windowSwitchWM);
    connect(widget, &PersonalizationGeneral::requestSetActiveColor, m_work, &dcc::personalization::PersonalizationWork::setActiveColor);
    m_work->active();

    m_frameProxy->pushWidget(this, widget);
    widget->setVisible(true);

    connect(widget, &PersonalizationGeneral::windowRadiusChanged, m_work, &dcc::personalization::PersonalizationWork::setWindowRadius);
}

void PersonalizationModule::showIconThemeWidget()
{
    m_work->refreshTheme();

    PerssonalizationThemeList *widget = new PerssonalizationThemeList;
    widget->setVisible(false);
    widget->setModel(m_model->getIconModel());
    connect(widget, &PerssonalizationThemeList::requestSetDefault, m_work, &dcc::personalization::PersonalizationWork::setDefault);
    m_work->active();

    m_frameProxy->pushWidget(this, widget);
    widget->setVisible(true);
}

void PersonalizationModule::showCursorThemeWidget()
{
    m_work->refreshTheme();

    PerssonalizationThemeList *widget = new PerssonalizationThemeList;
    widget->setVisible(false);
    widget->setModel(m_model->getMouseModel());
    connect(widget, &PerssonalizationThemeList::requestSetDefault, m_work, &dcc::personalization::PersonalizationWork::setDefault);
    m_work->active();

    m_frameProxy->pushWidget(this, widget);
    widget->setVisible(true);
}

void PersonalizationModule::showFontThemeWidget()
{
    m_work->refreshFont();

    PersonalizationFontsWidget *widget = new PersonalizationFontsWidget;
    widget->setVisible(false);
    widget->setModel(m_model);

    connect(widget, &PersonalizationFontsWidget::requestSetFontSize, m_work, &dcc::personalization::PersonalizationWork::setFontSize);
    connect(widget, &PersonalizationFontsWidget::requestSetDefault, m_work, &dcc::personalization::PersonalizationWork::setDefault);
    m_work->active();

    m_frameProxy->pushWidget(this, widget);
    widget->setVisible(true);
}

void PersonalizationModule::initSearchData()
{
    const QString& module = displayName();
    const QString& font = tr("Font");
    const QString& general = tr("General");
    const QString& iconTheme = tr("Icon Theme");
    const QString& cursorTheme = tr("Cursor Theme");

    static QMap<QString, bool> gsettingsMap;

    auto func_is_visible = [=](const QString &gsettings, bool isBool = true) {
        if (gsettings == "") {
            return false;
        }

        bool ret = false;
        if (isBool) {
            ret = GSettingWatcher::instance()->get(gsettings).toBool();
        } else {
            ret = GSettingWatcher::instance()->get(gsettings).toString() != "Hidden";
        }
        gsettingsMap.insert(gsettings, ret);

        return ret;
    };

    auto func_font_changed = [ = ](bool bFont) {
        m_frameProxy->setWidgetVisible(module, font, bFont);
        m_frameProxy->setDetailVisible(module, font, tr("Size"), bFont);
        m_frameProxy->setDetailVisible(module, font, tr("Standard Font"), bFont);
        m_frameProxy->setDetailVisible(module, font, tr("Monospaced Font"), bFont);
    };

    auto func_general_changed = [ = ](bool bGeneral) {
        bool bEffects = func_is_visible("perssonalGeneralEffects", false);
        bool is3DWm = m_model->is3DWm();

        m_frameProxy->setWidgetVisible(module, general, bGeneral);
        m_frameProxy->setDetailVisible(module, general, tr("Theme"), bGeneral && func_is_visible("perssonalGeneralThemes", false));
        m_frameProxy->setDetailVisible(module, general, tr("Light"), bGeneral);
        m_frameProxy->setDetailVisible(module, general, tr("Dark"), bGeneral);
        m_frameProxy->setDetailVisible(module, general, tr("Auto"), bGeneral);
        m_frameProxy->setDetailVisible(module, general, tr("Accent Color"), bGeneral);
        m_frameProxy->setDetailVisible(module, general, tr("Window Effect"), bGeneral && bEffects && !IsServerSystem);
        m_frameProxy->setDetailVisible(module, general, tr("Transparency"), bGeneral && bEffects && is3DWm);
        m_frameProxy->setDetailVisible(module, general, tr("Window Minimize Effect"), bGeneral && bEffects && is3DWm);
    };

    auto func_icontheme_changed = [ = ](bool bIconTheme) {
        m_frameProxy->setWidgetVisible(module, iconTheme, bIconTheme);
        m_frameProxy->setDetailVisible(module, iconTheme, iconTheme, bIconTheme);
    };

    auto func_cursortheme_changed = [ = ](bool bCursorTheme) {

        m_frameProxy->setWidgetVisible(module, cursorTheme, bCursorTheme);
        m_frameProxy->setDetailVisible(module, cursorTheme, cursorTheme, bCursorTheme);
    };

    auto func_process_all = [ = ]() {
        func_general_changed(func_is_visible("personalizationGeneral"));

        func_icontheme_changed(func_is_visible("personalizationIconTheme"));

        func_cursortheme_changed(func_is_visible("personalizationCursorTheme"));

        func_font_changed(func_is_visible("personalizationFont"));
     };

    QStringList gslist;
    gslist << "personalizationGeneral" << "personalizationIconTheme" << "personalizationCursorTheme" << "personalizationFont";

    connect(GSettingWatcher::instance(), &GSettingWatcher::notifyGSettingsChanged, this, [=](const QString &gsetting, const QString &state) {

        if (gsetting == "" || !gsettingsMap.contains(gsetting)) {
            return;
        }

        QVariant value = GSettingWatcher::instance()->get(gsetting);
        if (gslist.contains(gsetting) && (gsettingsMap.value(gsetting) == value.toBool())) {
            return;
        }

        if (("perssonalGeneralThemes" == gsetting || "perssonalGeneralEffects" == gsetting)
                && (gsettingsMap.value(gsetting) == (value.toString() != "Hidden"))) {
            return;
        }

        if ("personalizationGeneral" == gsetting) {
            func_general_changed(func_is_visible("personalizationGeneral"));
        } else if ("personalizationIconTheme" == gsetting) {
            func_icontheme_changed(func_is_visible("personalizationIconTheme"));
        } else if ("personalizationCursorTheme" == gsetting) {
            func_cursortheme_changed(func_is_visible("personalizationCursorTheme"));
        } else if ("personalizationFont" == gsetting) {
            func_font_changed(func_is_visible("personalizationFont"));
        } else if ("perssonalGeneralThemes" == gsetting) {
            func_general_changed(func_is_visible("personalizationGeneral"));
        } else if ("perssonalGeneralEffects" == gsetting) {
            func_general_changed(func_is_visible("personalizationGeneral"));
        } else {
            qInfo() << " not contains the gsettings : " << gsetting << state;
            return;
        }

        m_frameProxy->updateSearchData(module);
    });

    connect(m_model, &dcc::personalization::PersonalizationModel::wmChanged, this, [ = ](const bool is3d){
        bool bGeneral = func_is_visible("personalizationGeneral");
        bool bEffects = func_is_visible("perssonalGeneralEffects", false);
        m_frameProxy->setDetailVisible(module, general, tr("Transparency"), bGeneral && bEffects && is3d);
        m_frameProxy->setDetailVisible(module, general, tr("Window Minimize Effect"), bGeneral && bEffects && is3d);
        m_frameProxy->updateSearchData(module);
    });

    func_process_all();
}
