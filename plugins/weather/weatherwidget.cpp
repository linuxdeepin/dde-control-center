#include "weatherwidget.h"
#include "weatheritem.h"
#include <QPainter>
#include <QGraphicsPathItem>
#include <QGuiApplication>

DWIDGET_USE_NAMESPACE

WeatherWidget::WeatherWidget(QWidget *parent)
    :QWidget(parent),
      m_request(new WeatherRequest(this))
{
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

            text = QString("%1°C").arg(int(item.temperature().first));
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
            text = QString("%1-%2°C").arg((int)(item.temperature().first)).arg((int)(item.temperature().second));
            QRect textRect(iconRect.right()+10,rect.y(),fm.width(text)+2, rect.height());
            painter.drawText(textRect,Qt::AlignLeft|Qt::AlignVCenter, text);

            QRect weekArea(rect.width() - 50 - iconRect.left() -10,rect.y(),fm.width(item.dayName()), rect.height());
            painter.drawText(weekArea, Qt::AlignCenter, item.dayName());
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
    return QString(":/icon/%1.svg").arg(item.name());
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
