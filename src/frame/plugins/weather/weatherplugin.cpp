/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "weatherplugin.h"

#include <QStackedLayout>

#include "weatherwidget.h"
#include "setlocationpage.h"
#include "weatherrequest.h"

WeatherPlugin::WeatherPlugin(QObject *parent)
    : QObject(parent),
      m_view(new QFrame),
      m_requestManager(new WeatherRequest(this))
{
    WeatherWidget *weatherWidget = new WeatherWidget(m_requestManager);
    SetLocationPage *locationPage = new SetLocationPage(m_requestManager);

    QStackedLayout *layout = new QStackedLayout;
    layout->setSpacing(0);
    layout->setMargin(0);

    layout->addWidget(weatherWidget);
    layout->addWidget(locationPage);

    m_view->setLayout(layout);
    m_view->setObjectName("WeatherView");
    m_view->setStyleSheet("#WeatherView {"
                          "background-color: rgba(255, 255, 255, .03);"
                          "}");

    connect(weatherWidget, &WeatherWidget::locationButtonClicked, this, [this, layout, locationPage] {
        locationPage->reset();
        locationPage->setCurrentCity(m_requestManager->city());
        layout->setCurrentWidget(locationPage);
    });

    connect(locationPage, &SetLocationPage::citySet, this, [this, layout, weatherWidget] (const QString &preferredService, const City &city) {
        qDebug() << "set city to " << city.localizedName;
        QList<WeatherItem> empty;
        weatherWidget->refreshView(empty);
        m_requestManager->setPreferredWeatherService(preferredService);
        m_requestManager->setCity(city);
        layout->setCurrentWidget(weatherWidget);
    });

    connect(locationPage, &SetLocationPage::cancelled, this, [=] {
        qDebug() << "cancelled setting city" ;
        layout->setCurrentWidget(weatherWidget);
    });
}

void WeatherPlugin::initialize(FrameProxyInterface *proxy)
{
    m_proxyInter = proxy;
}

QWidget *WeatherPlugin::centralWidget()
{
    return m_view;
}
