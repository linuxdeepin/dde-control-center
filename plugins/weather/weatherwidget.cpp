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

#include "weatherwidget.h"
#include "weatheritem.h"
#include <QPainter>
#include <QGraphicsPathItem>
#include <QGuiApplication>
#include <QImageReader>
#include <QIcon>

static const int ItemRightMargin = 30;

static const int CurrentItemHeight = 80;
static const int ForecastItemHeight = 60;

static const int LoadingBaselineOffset = -30;

WeatherWidget::WeatherWidget(WeatherRequest *request, QWidget *parent)
    : QWidget(parent)
    , m_request(request)
    , m_refreshTipsTimer(new QTimer(this))
{
    setMouseTracking(true);

    m_view = new DPictureSequenceView(this);
    m_view->setFixedSize(50,50);
    QStringList lists;
    for(int i = 99; i >= 0; i--)
    {
        QString arg = QString().setNum(i);
        if(arg.length() == 1)
            arg = "0"+arg;
        QString path = QString(":/icon/load_weather/load_weather_000%1").arg(arg);
        lists<<path;
    }
    m_view->setPictureSequence(lists);


    m_locationBtn = new DImageButton(this);
    m_locationBtn->setNormalPic(":/icon/location_normal.png");
    m_locationBtn->setHoverPic(":/icon/location_hover.png");
    m_locationBtn->setPressPic(":/icon/location_press.png");
    m_locationBtn->setVisible(false);

    m_refreshTipsTimer->setInterval(60 * 1000);
    m_refreshTipsTimer->setSingleShot(true);

    connect(m_locationBtn, &DImageButton::clicked, this, &WeatherWidget::locationButtonClicked);

    connect(m_request, SIGNAL(dataRefreshed(QList<WeatherItem>&)),
            this, SLOT(refreshView(QList<WeatherItem>&)));
    connect(m_request, SIGNAL(fetchLocalizedCityNameDone(QString)), this, SLOT(update()));

    connect(m_refreshTipsTimer, &QTimer::timeout, this, static_cast<void (WeatherWidget::*)()>(&WeatherWidget::update));

    m_request->sendRefreshSignal();
}

void WeatherWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    m_request->refreshData();
}

void WeatherWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    int count = m_items.count();
    const qreal ratio = devicePixelRatioF();
    if(count == 0)
    {
        QPainter painter(this);
        QPixmap pix = loadPixmap(":/icon/load_weather/weather_loading.svg");

        QRect loadingRect(rect().center() + QPoint(-pix.width()/2/ratio, -pix.height()/2/ratio + LoadingBaselineOffset),
                          pix.size() / devicePixelRatioF());
        painter.drawPixmap(loadingRect, pix);
        /*
        QString arg = QString().setNum(i);
        if(arg.length() == 1)
            arg = "0"+arg;
        QString path = QString(":/icon/load_weather/load_weather_000%1").arg(arg);
        QPixmap rotate(path);
        painter.drawPixmap(rect().center() + QPoint(-rotate.width()/2, -rotate.height()/2), rotate);
        --i;
        i = (i<0)?99:i;
        */
        QTime time = QTime::currentTime();
        QRect rect(0,0,width(), 50);
        rect.moveTop(loadingRect.bottom()+10);

        QPen pen(Qt::white);
        painter.setPen(pen);
        if(m_time.secsTo(time) > 10)
        {
            painter.drawText(rect, Qt::AlignCenter, tr("Sorry, unable to get weather!"));
            m_locationBtn->move(rect.x() + (rect.width() - m_locationBtn->width()) / 2, rect.bottom());
            m_locationBtn->show();
            update();
        }
        else
        {
            painter.drawText(rect, Qt::AlignCenter, tr("Loading weather..."));
        }
        return;
    }

    int h = ForecastItemHeight;
    int fh = CurrentItemHeight;
    int y = 0;

    QPainter painter(this);
    QColor col = QColor(qrand()%255, qrand()%255, qrand()%255, 100);
    QColor pen1(Qt::white);
    painter.setPen(pen1);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(col);
//    painter.drawRect(rect());
    for(int i= 0; i<count; i++)
    {
        QRect rect(0,y,width(),((i == 0) ? fh : h));
        QPoint center = rect.center();
//        painter.drawRect(rect);
        WeatherItem item = m_items[i];

        QPixmap icon = loadPixmap(this->icon(item));
        QRect iconRect(0,0,icon.width() / ratio, icon.height() / ratio);
        iconRect.moveCenter(QPoint(center.x()/3, center.y()));
        painter.drawPixmap(iconRect, icon);

        QFont curFont = QGuiApplication::font();
        QString text;
        if(i == 0)
        {
            painter.save();
            painter.setRenderHint(QPainter::Antialiasing);
            QFont font = painter.font();
            font.setPointSize(curFont.pointSize()*1.5);
            QFontMetrics fm(font);
            QPen pen(Qt::white);
            painter.setPen(pen);
            painter.setFont(font);

            const int temperature = int(item.temperature().first);

            if (item.isFahrenheit())
                text = QString("%1°F").arg(32 + temperature * 1.8);
            else
                text = QString("%1°C").arg(temperature);

            QRect textRect(iconRect.right()+10,rect.y(),fm.width(text)+2, rect.height());
            painter.drawText(textRect,Qt::AlignLeft|Qt::AlignVCenter, text);
            font.setPointSize(curFont.pointSize()*0.65);
            painter.setFont(font);

            fm=QFontMetrics(font);
            int descWidth = fm.width(item.description()) > 101 ? 101 : fm.width(item.description());
            QRect descRect(textRect.right(), textRect.top()+2, descWidth, rect.height());
            painter.drawText(descRect, Qt::AlignLeft | Qt::AlignVCenter, item.description());

            if (!m_locationBtn->isVisible()) {
                font.setPointSize(curFont.pointSize() * 1.4);
                painter.setFont(font);
                fm=QFontMetrics(font);
                QString city = m_request->localizedCityName();
                QRect cityArea(descRect.right(), rect.y(), rect.width() - descRect.right() - ItemRightMargin, rect.height()/2+6);
//                painter.fillRect(cityArea, Qt::red);
                painter.drawText(cityArea, Qt::AlignBottom | Qt::AlignRight, fm.elidedText(city, Qt::ElideRight, cityArea.width()));

                if (m_refreshTipsTimer->isActive()) {
                    font.setPointSize(curFont.pointSize() * 0.8);
                    fm=QFontMetrics(font);
                    QString updateTime = tr("Just updated");
                    QRect statusArea(descRect.right(), cityArea.bottom(), rect.width() - descRect.right() - ItemRightMargin, rect.height()/2 -6);
                    pen1.setAlphaF(0.5);
                    painter.setPen(pen1);
                    painter.setFont(font);
                    painter.drawText(statusArea, Qt::AlignTop|Qt::AlignRight, updateTime);
                }
            }

            painter.restore();
        }
        else
        {
            painter.save();
            painter.setFont(curFont);
            painter.setRenderHint(QPainter::Antialiasing);
            QFontMetrics fm(curFont);

            if (item.isFahrenheit())
                text = QString("%1-%2°F")
                    .arg((int)(32 + item.temperature().first * 1.8))
                    .arg((int)(32 + item.temperature().second * 1.8));
            else
                text = QString("%1-%2°C")
                        .arg((int)(item.temperature().first))
                        .arg((int)(item.temperature().second));

            QRect textRect(iconRect.right()+10,rect.y(),fm.width(text)+2, rect.height());
            painter.drawText(textRect,Qt::AlignLeft | Qt::AlignVCenter, text);

            QRect weekArea(textRect.right(), rect.y(), rect.width() - textRect.right() - ItemRightMargin, rect.height());
            painter.drawText(weekArea, Qt::AlignVCenter | Qt::AlignRight, item.dayName());
            painter.restore();
        }
        y+=rect.height();
    }
}

void WeatherWidget::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);

    const bool showButton = event->y() < CurrentItemHeight && event->y() > 10 && m_items.count() != 0;
    if (showButton) { m_locationBtn->move(width() - 60, 30); }
    m_locationBtn->setVisible(showButton);

    update();
}

void WeatherWidget::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);

    QRect rect(0,0,50,50);
    rect.moveCenter(this->rect().center() + QPoint(0, LoadingBaselineOffset));
    m_view->setGeometry(rect);
}

QString WeatherWidget::icon(const WeatherItem &item)
{
    return QString(":/icon/%1.svg").arg(item.name());
}

void WeatherWidget::refreshView(QList<WeatherItem> &items)
{
    m_items.clear();
    m_items = items;
    m_refreshTipsTimer->start();
    if(items.count() == 0)
    {
        m_view->show();
        m_view->play();
        if(!m_time.isValid())
            m_time = QTime::currentTime();
    }
    else
    {
        m_view->hide();
        m_view->stop();
        if(m_time.isValid())
            m_time = QTime();
    }

    m_locationBtn->setVisible(false);
    update();
}

QPixmap WeatherWidget::loadPixmap(const QString &path)
{
    qreal ratio = 1.0;
    QPixmap pixmap;

    const qreal devicePixelRatio = qApp->devicePixelRatio();

    if (!qFuzzyCompare(ratio, devicePixelRatio)) {
        QImageReader reader;
        reader.setFileName(qt_findAtNxFile(path, devicePixelRatio, &ratio));
        if (reader.canRead()) {
            reader.setScaledSize(reader.size() * (devicePixelRatio / ratio));
            pixmap = QPixmap::fromImage(reader.read());
            pixmap.setDevicePixelRatio(devicePixelRatio);
        }
    } else {
        pixmap.load(path);
    }

    return pixmap;
}
