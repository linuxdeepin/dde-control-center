#include "weatherplugin.h"

#include <QStackedLayout>

#include "weatherwidget.h"
#include "setlocationpage.h"
#include "weatherrequest.h"

WeatherPlugin::WeatherPlugin(QObject *parent)
    : QObject(parent),
      m_view(new QWidget),
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

    connect(weatherWidget, &WeatherWidget::locationButtonClicked, this, [this, layout, locationPage] {
        locationPage->setCurrentCity(m_requestManager->city());
        layout->setCurrentWidget(locationPage);
    });

    connect(locationPage, &SetLocationPage::citySet, this, [this, layout, weatherWidget] (const City &city) {
        qDebug() << "set city to " << city.localizedName;
        m_requestManager->setCity(city);
        layout->setCurrentWidget(weatherWidget);
    });

    connect(locationPage, &SetLocationPage::cancelled, this, [this, layout, weatherWidget] {
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
