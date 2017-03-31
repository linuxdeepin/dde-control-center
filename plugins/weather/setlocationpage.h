/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SETLOCATIONPAGE_H
#define SETLOCATIONPAGE_H

#include <QWidget>
#include <QListView>
#include <QAbstractListModel>
#include <QStyledItemDelegate>

#include "searchinput.h"
#include "labels/largelabel.h"
#include "types.h"

using namespace dcc::widgets;

class SearchResultView : public QListView
{
    Q_OBJECT
public:
    explicit SearchResultView(QWidget *parent = 0);
    virtual ~SearchResultView() {}
};

class SearchModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit SearchModel(QObject *parent = 0);
    virtual ~SearchModel() {}

    void setCities(const QList<City> &cities);
    QList<City> cities() const;

private:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    QList<City> m_cities;
};

class SearchDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit SearchDelegate(QObject *parent = 0);
    virtual ~SearchDelegate() {}

private:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};


class WeatherRequest;
class LocationProvider;
class SetLocationPage : public QWidget
{
    Q_OBJECT
public:
    SetLocationPage(WeatherRequest *requestManager, QWidget *parent = 0);

    City currentCity() const;
    void setCurrentCity(const City &currentCity);

    void reset() const;

signals:
    void citySet(const QString &prferredService, const City &city);
    void cancelled();

private:
    void mouseReleaseEvent(QMouseEvent *event);

    void loadSupportedCities();
    void setSearchResult(const QList<City> data);

private:
    City m_currentCity;
    QString m_preferredWeatherService;
    WeatherRequest *m_requestManager;

    QList<LocationProvider*> m_locationProviders;

    LargeLabel *m_currentCityLabel;
    SearchInput *m_searchInput;
    SearchResultView *m_resultView;
    SearchDelegate *m_resultDelegate;
    SearchModel *m_resultModel;
    LargeLabel *m_noResult;
    QStringList m_supportedCities;

    QTimer *m_searchTimer;
};

#endif // SETLOCATIONPAGE_H
