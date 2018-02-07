/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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

#include "setlocationpage.h"

#include <QVBoxLayout>
#include <QPainter>
#include <QListView>
#include <QTimer>
#include <QFile>
#include <QStringList>

#include "weatherrequest.h"

#include "caiyun/caiyunlocationprovider.h"
#include "deepin/deepinlocationprovider.h"

static const int ItemSpacing = 1;

SetLocationPage::SetLocationPage(WeatherRequest *requestManager, QWidget *parent)
    : QWidget(parent),
      m_requestManager(requestManager),
      m_currentCityLabel(new LargeLabel),
      m_searchInput(new SearchInput),
      m_resultView(new SearchResultView),
      m_resultDelegate(new SearchDelegate),
      m_resultModel(new SearchModel),
      m_noResult(new LargeLabel(tr("No search results"))),
      m_searchTimer(new QTimer)
{
    m_searchInput->setFixedHeight(36);
    m_searchInput->setFixedWidth(340);
    m_resultView->setFixedWidth(340);
    m_noResult->setStyleSheet("color: rgba(255, 255, 255, 0.3);");

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addSpacing(20);
    layout->addWidget(m_currentCityLabel, 0, Qt::AlignHCenter);
    layout->addSpacing(20);
    layout->addWidget(m_searchInput, 0, Qt::AlignHCenter);
    layout->addSpacing(10);
    layout->addWidget(m_resultView, 1, Qt::AlignHCenter);
    layout->addStretch();

    setLayout(layout);

    QHBoxLayout *resultLayout = new QHBoxLayout;
    resultLayout->setSpacing(0);
    resultLayout->setMargin(0);

    resultLayout->addStretch();
    resultLayout->addWidget(m_noResult);
    resultLayout->addStretch();

    m_resultView->setLayout(resultLayout);

    m_resultView->setItemDelegate(m_resultDelegate);
    m_resultView->setModel(m_resultModel);
    m_resultView->hide();

    m_searchTimer->setSingleShot(true);
    m_searchTimer->setInterval(1000);

    m_locationProviders << new CaiyunLocationProvider << new DeepinLocationProvider;
    for (LocationProvider * provider : m_locationProviders) {
        provider->loadData();
    }

    connect(m_searchTimer, &QTimer::timeout, this, [this] {
        const QString input = m_searchInput->text().trimmed();

        // try local location provider first.
        QList<City> result;
        for (LocationProvider *provider : m_locationProviders) {
            result = provider->match(input);
            if (!result.isEmpty()) {
                m_preferredWeatherService = provider->preferredWeatherService();
                setSearchResult(result);
                break;
            }
        }

        if (result.isEmpty()) {
            m_requestManager->searchCity(input);
        }
    });

    connect(m_searchInput, &SearchInput::textChanged, this, [this] {
        m_resultView->hide();
        m_resultModel->setCities(QList<City>());
        m_searchTimer->start();
    });

    connect(m_requestManager, &WeatherRequest::searchCityDone, this, &SetLocationPage::setSearchResult);

    connect(m_resultView, &SearchResultView::clicked, this, [this](const QModelIndex &index) {
        QVariant data = index.data(Qt::UserRole);
        QString geonameId = data.value<QString>();

        for (const City &city : m_resultModel->cities()) {
            if (city.id == geonameId) {
                emit citySet(m_preferredWeatherService, city);
                break;
            }
        }
    });
}

City SetLocationPage::currentCity() const
{
    return m_currentCity;
}

void SetLocationPage::setCurrentCity(const City &currentCity)
{
    m_currentCity = currentCity;
    const QString text  =  tr("Current City: %1").arg(m_currentCity.localizedName);

    QFontMetrics fm = m_currentCityLabel->fontMetrics();
    const QString elidedText = fm.elidedText(text, Qt::ElideRight, 320);

    m_currentCityLabel->setText(elidedText);
}

void SetLocationPage::reset() const
{
    m_searchInput->setText("");
    m_searchInput->clearFocus();
}

void SetLocationPage::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);

    emit cancelled();
}

void SetLocationPage::loadSupportedCities()
{
    if (m_supportedCities.isEmpty()) {
        QFile file(":/supported_cities.txt");
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            QString content = file.readAll();
            m_supportedCities = content.split(",");
            qDebug() << m_supportedCities.length();
            file.close();
        }

    }
}

void SetLocationPage::setSearchResult(const QList<City> data)
{
    if (!m_searchInput->text().trimmed().isEmpty()) {

        QList<City> buffer;
        for (const City &city : data) {
            if (buffer.indexOf(city) == -1) {
                buffer << city;
            }
        }

        m_noResult->setVisible(buffer.length() == 0);
        m_resultModel->setCities(buffer);
        m_resultView->show();
    }
}

SearchModel::SearchModel(QObject *parent) :
    QAbstractListModel(parent)
{

}

int SearchModel::rowCount(const QModelIndex &) const
{
    return m_cities.length();
}

QVariant SearchModel::data(const QModelIndex &index, int role) const
{
    const City city = m_cities.at(index.row());

    if (role == Qt::UserRole) {
        return city.id;
    }

    if (role == Qt::ToolTipRole) {
        return "";
    }

    return QString("%1, %2").arg(city.localizedName).arg(city.country);
}

QList<City> SearchModel::cities() const
{
    return m_cities;
}

void SearchModel::setCities(const QList<City> &cities)
{
    beginResetModel();
    m_cities = cities;
    endResetModel();
}

SearchDelegate::SearchDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}

void SearchDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QVariant textData = index.data(Qt::DisplayRole);
    QString cityName = textData.value<QString>();

    QRect rect( option.rect.adjusted(0, 0, 0, -ItemSpacing) );
    painter->fillRect(rect, QColor::fromRgbF(1, 1, 1, 0.15));

    QTextOption opt;
    opt.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    painter->setPen(Qt::white);
    rect = rect.adjusted(20, 0, 20, 0);
    painter->drawText(rect, cityName, opt);
}

QSize SearchDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(36 + ItemSpacing);

    return size;
}

SearchResultView::SearchResultView(QWidget *)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyleSheet("background: transparent; margin: 0px; border: 0px; padding: 0px;");
}
