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

#ifndef WEATHERWIDGET_H
#define WEATHERWIDGET_H

#include <QWidget>
#include "weatherrequest.h"
#include <QTimer>
#include <QResizeEvent>
#include <dpicturesequenceview.h>
#include <dimagebutton.h>

DWIDGET_USE_NAMESPACE

class WeatherWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WeatherWidget(WeatherRequest *request, QWidget* parent = 0);

signals:
    void locationButtonClicked();

protected:
    void showEvent(QShowEvent *event);
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent* e);
    QString icon(const WeatherItem& item);
    QPixmap loadPixmap(const QString &path);

public slots:
    void refreshView(QList<WeatherItem> &items);

private:
    QTime m_time;
    QTimer *m_refreshTipsTimer;
    WeatherRequest *m_request;
    QList<QPixmap> m_pixmaps;
    DPictureSequenceView* m_view;
    DImageButton *m_locationBtn;
    QList<WeatherItem> m_items;
};

#endif // WEATHERWIDGET_H
