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
#include "wallpaperpage.h"

#include <DApplicationHelper>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::personalization;

PersonalizationModule::PersonalizationModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_model(nullptr)
    , m_work(nullptr)
{
}

PersonalizationModule::~PersonalizationModule()
{
    if (m_model)
        m_model->deleteLater();
    if (m_work)
        m_work->deleteLater();
}

void PersonalizationModule::initialize()
{
    if (m_model) {
        delete m_model;
    }
    m_model  = new dcc::personalization::PersonalizationModel;
    m_work = new dcc::personalization::PersonalizationWork(m_model);

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
    if (DGuiApplicationHelper::isTabletEnvironment()) {
        QWidget *w = new QWidget(); // 需要匹配控制中心三级显示
        w->setVisible(false);
        m_frameProxy->pushWidget(this, w);
        showWallpaperPage();
    } else {
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
        //显示默认页
        firstWidget->setDefaultWidget();
    }
}

void PersonalizationModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}

int PersonalizationModule::load(const QString &path)
{
    QString loadPath = path.split("/").at(0);
    int row = -1;
    if (DGuiApplicationHelper::isTabletEnvironment()) {
        showWallpaperPage();
        row = 0;
    } else {
        if (loadPath == QStringLiteral("General")) {
            showGenaralWidget();
            row = 0;
        } else if (loadPath == QStringLiteral("Icon Theme")) {
            showIconThemeWidget();
            row = 1;
        } else if (loadPath == QStringLiteral("Cursor Theme")) {
            showCursorThemeWidget();
            row = 2;
        } else if (loadPath == QStringLiteral("Font")) {
            showFontThemeWidget();
            row = 3;
        }
    }

    Q_EMIT requestSetCurrentIndex(row);
    return row == -1 ? -1 : 0;
}

QStringList PersonalizationModule::availPage() const
{
    QStringList sl;
    sl << "General" << "Icon Theme" << "Cursor Theme" << "Font";
    return sl;
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

void PersonalizationModule::showWallpaperPage()
{
    m_work->refreshTheme();
    m_work->refreshFont();

    WallpaperPage *widget = new WallpaperPage();
    widget->setAccessibleName("personanization");
    connect(widget->getThemeWidget(), &PerssonalizationThemeWidget::requestSetDefault, m_work, &dcc::personalization::PersonalizationWork::setDefault);
    connect(widget, &WallpaperPage::requestSetActiveColor, m_work, &dcc::personalization::PersonalizationWork::setActiveColor);
    connect(widget->getFontWidget(), &PersonalizationFontsWidget::requestSetFontSize, m_work, &dcc::personalization::PersonalizationWork::setFontSize);
    connect(widget->getFontWidget(), &PersonalizationFontsWidget::requestSetDefault, m_work, &dcc::personalization::PersonalizationWork::setDefault);

    widget->setVisible(false);
    widget->setModel(m_model);
    widget->setWorker(m_work);
    m_work->active();

    m_frameProxy->pushWidget(this, widget);
    widget->setVisible(true);
}
