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
#include "personalizationicontheme.h"
#include "personalizationcursortheme.h"
#include "personalizationfontswidget.h"

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
    m_model->deleteLater();
    m_work->deleteLater();
}

void PersonalizationModule::initialize()
{
    m_model  = new dcc::personalization::PersonalizationModel;
    m_work = new dcc::personalization::PersonalizationWork(m_model);

    m_model->moveToThread(qApp->thread());
    m_work->moveToThread(qApp->thread());
}

const QString PersonalizationModule::name() const
{
    return QStringLiteral("personalization");
}

void PersonalizationModule::active()
{
    PersonalizationList *firstWidget = new PersonalizationList();

    connect(firstWidget, &PersonalizationList::requestShowGeneral, this, &PersonalizationModule::showGenaralWidget);
    connect(firstWidget, &PersonalizationList::requestShowIconTheme, this, &PersonalizationModule::showIconThemeWidget);
    connect(firstWidget, &PersonalizationList::requestShowCursorTheme, this, &PersonalizationModule::showCursorThemeWidget);
    connect(firstWidget, &PersonalizationList::requestShowFonts, this, &PersonalizationModule::showFontThemeWidget);
    m_frameProxy->pushWidget(this, firstWidget);
    //显示默认页
    showGenaralWidget();
}

void PersonalizationModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}

void PersonalizationModule::showGenaralWidget()
{
    m_work->refreshTheme();

    PersonalizationGeneral *widget = new PersonalizationGeneral;

    widget->setModel(m_model);
    connect(widget->getThemeWidget(), &PerssonalizationThemeWidget::requestSetDefault, m_work, &dcc::personalization::PersonalizationWork::setDefault);
    connect(widget, &PersonalizationGeneral::requestSetOpacity, m_work, &dcc::personalization::PersonalizationWork::setOpacity);
    connect(widget, &PersonalizationGeneral::requestSwitchWM, m_work, &dcc::personalization::PersonalizationWork::switchWM);
    connect(widget, &PersonalizationGeneral::requestSetActiveColor, m_work, &dcc::personalization::PersonalizationWork::setActiveColor);
    m_work->active();

    m_frameProxy->pushWidget(this, widget);
}

void PersonalizationModule::showIconThemeWidget()
{
    m_work->refreshTheme();

    PersonalizationIconTheme *widget = new PersonalizationIconTheme;
    widget->setModel(m_model->getIconModel());
    connect(widget, &PersonalizationIconTheme::requestSetDefault, m_work, &dcc::personalization::PersonalizationWork::setDefault);
    m_work->active();

    m_frameProxy->pushWidget(this, widget);
}

void PersonalizationModule::showCursorThemeWidget()
{
    m_work->refreshTheme();

    PersonalizationCursorTheme *widget = new PersonalizationCursorTheme;
    widget->setModel(m_model->getMouseModel());
    connect(widget, &PersonalizationCursorTheme::requestSetDefault, m_work, &dcc::personalization::PersonalizationWork::setDefault);
    m_work->active();

    m_frameProxy->pushWidget(this, widget);
}

void PersonalizationModule::showFontThemeWidget()
{
    m_work->refreshFont();

    PersonalizationFontsWidget *widget = new PersonalizationFontsWidget;
    widget->setModel(m_model);

    connect(widget, &PersonalizationFontsWidget::requestSetFontSize, m_work, &dcc::personalization::PersonalizationWork::setFontSize);
    connect(widget, &PersonalizationFontsWidget::requestSetDefault, m_work, &dcc::personalization::PersonalizationWork::setDefault);
    m_work->active();

    m_frameProxy->pushWidget(this, widget);
}
