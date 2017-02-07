#include "weatherwidget.h"
#include "weatheritem.h"
#include <QPainter>
#include <networkutil.h>
#include <QGraphicsPathItem>
#include <QGuiApplication>

DWIDGET_USE_NAMESPACE

WeatherWidget::WeatherWidget(QWidget *parent)
    :QWidget(parent),
      m_request(new WeatherRequest(this))
{
//    setFixedSize(300, 540);

    m_iconsDict["thunderstorm with light rain"] = ":/icon/thunderstorm";
    m_iconsDict["thunderstorm with rain"] = ":/icon/thunderstorm";
    m_iconsDict["thunderstorm with heavy rain"] = ":/icon/heavy thunderstorm";
    m_iconsDict["light thunderstorm"] = ":/icon/thunderstorm";
    m_iconsDict["thunderstorm"] = ":/icon/thunderstorm";
    m_iconsDict["heavy thunderstorm"] = ":/icon/heavy thunderstorm";
    m_iconsDict["ragged thunderstorm"] = ":/icon/thunderstorm";
    m_iconsDict["thunderstorm with light drizzle"] =":/icon/thunderstorm";
    m_iconsDict["thunderstorm with drizzle"] = ":/icon/thunderstorm";
    m_iconsDict["thunderstorm with heavy drizzle"] = ":/icon/thunderstorm";

    m_iconsDict["light intensity drizzle"] = ":/icon/drizzle";
    m_iconsDict["drizzle"] = ":/icon/drizzle";
    m_iconsDict["heavy intensity drizzle"] = ":/icon/moderate rain";
    m_iconsDict["light intensity drizzle rain"] = ":/icon/drizzle";
    m_iconsDict["drizzle rain"] = ":/icon/drizzle";
    m_iconsDict["heavy intensity drizzle rain"] = ":/icon/moderate rain";
    m_iconsDict["shower rain and drizzle"] = ":/icon/shower rain";
    m_iconsDict["heavy shower rain and drizzle"] = ":/icon/shower rain";
    m_iconsDict["shower drizzle"] = ":/icon/shower rain";

    m_iconsDict["light rain"] = ":/icon/drizzle";
    m_iconsDict["moderate rain"] = ":/icon/moderate rain";
    m_iconsDict["heavy intensity rain"] = ":/icon/heavy intensity rain";
    m_iconsDict["very heavy rain"] = ":/icon/very heavy rain";
    m_iconsDict["extreme rain"] = ":/icon/extreme rain";
    m_iconsDict["freezing rain"] = ":/icon/freezing rain";
    m_iconsDict["light intensity shower rain"] = ":/icon/shower rain";
    m_iconsDict["shower rain"] = ":/icon/shower rain";
    m_iconsDict["heavy intensity shower rain"] = ":/icon/shower rain";
    m_iconsDict["ragged shower rain"] = ":/icon/shower rain";

    m_iconsDict["light snow"] = ":/icon/light snow";
    m_iconsDict["snow"] = ":/icon/snow";
    m_iconsDict["heavy snow"] = ":/icon/heavy snow";
    m_iconsDict["sleet"] = ":/icon/sleet";
    m_iconsDict["shower sleet"] = ":/icon/sleet";
    m_iconsDict["light rain and snow"] = ":/icon/sleet";
    m_iconsDict["rain and snow"] = ":/icon/sleet";
    m_iconsDict["light shower snow"] = ":/icon/shower snow";
    m_iconsDict["shower snow"] = ":/icon/shower snow";
    m_iconsDict["heavy shower snow"] = ":/icon/shower snow";

    m_iconsDict["clear sky"] = ":/icon/clear sky";
    m_iconsDict["few clouds"] = ":/icon/few clouds";
    m_iconsDict["scattered clouds"] = ":/icon/few clouds";
    m_iconsDict["broken clouds"] = ":/icon/few clouds";
    m_iconsDict["overcast clouds"] = ":/icon/overcast clouds";

    m_iconsDict["tornado"] = ":/icon/tornado";
    m_iconsDict["tropical storm"] = ":/icon/tropical storm";
    m_iconsDict["hurricane"] = ":/icon/hurricane";
    m_iconsDict["cold"] = ":/icon/cold";
    m_iconsDict["hot"] = ":/icon/hot";
    m_iconsDict["windy"] = ":/icon/windy";
    m_iconsDict["hail"] = ":/icon/hail";
    m_iconsDict["fresh breeze"] = ":/icon/gale";
    m_iconsDict["strong breeze"] = ":/icon/gale";
    m_iconsDict["high wind, near gale"] = ":/icon/gale";
    m_iconsDict["gale"] = ":/icon/gale";
    m_iconsDict["severe gale"] = ":/icon/gale";
    m_iconsDict["storm"] = ":/icon/storm";
    m_iconsDict["violent storm"] = ":/icon/violent storm";

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

    connect(m_request, SIGNAL(refreshData(QList<WeatherItem>&)),
            this, SLOT(refreshView(QList<WeatherItem>&)));
    m_request->sendRefreshSignal();
}
void WeatherWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    int count = m_request->count();
    if(count == 0)
    {
        QPainter painter(this);
        QPixmap pix(":/icon/load_weather/weather_loading.png");
        painter.drawPixmap(rect().center() + QPoint(-pix.width()/2, -pix.height()/2), pix);
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
        rect.moveTop(this->rect().center().y()+pix.height()/2+25);

        QPen pen(Qt::white);
        painter.setPen(pen);
        if(m_time.secsTo(time) > 10)
        {
            painter.drawText(rect, Qt::AlignCenter, tr("Sorry, unable to get weather!"));
        }
        else
        {
            painter.drawText(rect, Qt::AlignCenter, tr("Loading weather..."));
        }
        return;
    }

    int h = (height() - 20)/count;
    int fh = h + 20;
    int y = 0;

    QPainter painter(this);
    QColor col = QColor(qrand()%255, qrand()%255, qrand()%255, 100);
    QPen pen1(Qt::white);
    painter.setPen(pen1);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(col);
//    painter.drawRect(rect());
    for(int i= 0; i<count; i++)
    {
        QRect rect(0,y,width(),((i == 0) ? fh : h));
        QPoint center = rect.center();
//        painter.drawRect(rect);
        WeatherItem item = m_request->dayAt(i);

        QPixmap icon = QPixmap(this->icon(item));
        QRect iconRect(0,0,icon.width(), icon.height());
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

            text = QString("%1°C").arg(int(item.day()+0.5));
            QRect textRect(iconRect.right()+10,rect.y(),fm.width(text)+2, rect.height());
            painter.drawText(textRect,Qt::AlignLeft|Qt::AlignVCenter, text);
            font.setPointSize(curFont.pointSize()*0.65);
            painter.setFont(font);

            fm=QFontMetrics(font);
            int descWidth = fm.width(item.description()) > 101 ? 101 : fm.width(item.description());
            QRect descRect(textRect.right(), textRect.top()+2, descWidth, rect.height());
            painter.drawText(descRect, Qt::AlignLeft | Qt::AlignVCenter, item.description());

            font.setPointSize(curFont.pointSize() * 1.4);
            painter.setFont(font);
            fm=QFontMetrics(font);
            QString city = m_request->city();
            QRect cityArea(rect.width() - 50 - iconRect.left()-10,rect.y(),fm.width(city), rect.height()/2+6);
            painter.drawText(cityArea, Qt::AlignBottom | Qt::AlignHCenter, city);
            font.setPointSize(curFont.pointSize() * 0.8);
            fm=QFontMetrics(font);
            QString updateTime = tr("Just updated");
            QRect statusArea(rect.width() - 50 - iconRect.left()-10,cityArea.bottom(),fm.width(updateTime), rect.height()/2 -6);
            painter.setFont(font);
            painter.drawText(statusArea, Qt::AlignTop|Qt::AlignHCenter, updateTime);
            painter.restore();
        }
        else
        {
            painter.save();
            painter.setFont(curFont);
            painter.setRenderHint(QPainter::Antialiasing);
            QFontMetrics fm(curFont);
            text = QString("%1-%2°C").arg((int)(item.min()+0.5)).arg((int)(item.max()+0.5));
            QRect textRect(iconRect.right()+10,rect.y(),fm.width(text)+2, rect.height());
            painter.drawText(textRect,Qt::AlignLeft|Qt::AlignVCenter, text);

            QRect weekArea(rect.width() - 50 - iconRect.left() -10,rect.y(),fm.width(item.dayOfWeek()), rect.height());
            painter.drawText(weekArea, Qt::AlignCenter, item.dayOfWeek());
            painter.restore();
        }
        y+=rect.height();
    }
}

void WeatherWidget::mousePressEvent(QMouseEvent *)
{
    update();
}

void WeatherWidget::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);

    QRect rect(0,0,50,50);
    rect.moveCenter(this->rect().center());
    m_view->setGeometry(rect);
}

QString WeatherWidget::icon(const WeatherItem &item)
{
    QStringList nights;
    nights<<"clear sky"<<"overcast clouds"<<"shower snow"
         <<"shower rain";
    if(nights.contains(item.description()))
    {
        QTime time = QTime::currentTime();
        if(time.hour() >= 18 || time.hour() <= 6)
        {
            return m_iconsDict[item.description()]+" night";
        }
        else
        {
            return m_iconsDict[item.description()];
        }
    }
    else
    {
        return m_iconsDict[item.description()];
    }
}

void WeatherWidget::refreshView(QList<WeatherItem> &items)
{
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

    update();
}
