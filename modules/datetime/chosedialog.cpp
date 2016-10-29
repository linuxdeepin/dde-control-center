#include "chosedialog.h"
#include "datetimeutil.h"

#include <cmath>
#include <QPainter>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QCompleter>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QTimeZone>
#include <QDebug>

ChoseDialog::ChoseDialog(QWidget *parent)
    :QDialog(parent)
{
    initData();
    QVBoxLayout* layout = new QVBoxLayout();
    QLabel *title = new QLabel;
    title->setText(tr("Add Timezone"));
    QFont font;
    font.setPixelSize(32);

    title->setFont(font);
    QLabel *sub = new QLabel;
    sub->setText(tr("please choose the timezone city by click map"));

    m_search = new QLineEdit();
    m_search->setText(tr("search"));
    m_search->setFixedWidth(300);
    QStringList citys;
    QList<Timezone*>::iterator it = m_timezones.begin();
    for(; it != m_timezones.end(); it++)
    {
        citys<<(*it)->m_city;
    }
    QCompleter *completer = new QCompleter(citys);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_search->setCompleter(completer);

    m_widget = new MapWidget();
    m_widget->setTimezones(m_timezones);
    m_cancel = new QPushButton(tr("Cancel"));
    m_cancel->setFixedWidth(200);
    m_add = new QPushButton(tr("Add"));
    m_add->setFixedWidth(200);

    layout->addWidget(title, 0, Qt::AlignHCenter);
    layout->addWidget(sub, 0, Qt::AlignHCenter);

    layout->addSpacerItem(new QSpacerItem(100,10));
    layout->addWidget(m_search, 0, Qt::AlignHCenter);
    layout->addSpacerItem(new QSpacerItem(100,10));

    layout->addWidget(m_widget, 0, Qt::AlignHCenter);
    layout->addWidget(m_cancel, 0, Qt::AlignHCenter);
    layout->addWidget(m_add, 0, Qt::AlignHCenter);

    setLayout(layout);
}

void ChoseDialog::initData()
{
    m_timezones.clear();
    QSqlDatabase db;
    if(QSqlDatabase::contains("timezone"))
    {
        db = QSqlDatabase::database(QLatin1String("timezone"));
    }
    else
    {
        db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), "timezone");
    }

    db.setDatabaseName(CITIES_DATABASE_PATH);

    if (!db.open())
    {
        qWarning() << QString("Failed to open %1 database").arg(db.databaseName());
        qWarning() << db.lastError();
        return;
    }

    QSqlQuery query(db);

    QString schema = QString("SELECT timezone, latitude, longitude, name from tzcity");

    if(!query.exec(schema))
    {
        qDebug()<<query.lastError();
        return ;
    }
    db.transaction();
    while(query.next())
    {
        QSqlRecord record = query.record();
        Timezone* timezone = new Timezone;
        for(int i = 0; i<record.count(); i++)
        {
            QSqlField field = record.field(i);
            if(field.name() == "timezone")
            {
                timezone->m_timezone = field.value().toString();
            }
            else if(field.name() == "latitude")
            {
                timezone->m_lat = field.value().toDouble();
            }
            else if(field.name() == "longitude")
            {
                timezone->m_lon = field.value().toDouble();
            }
            else
            {
                timezone->m_city = field.value().toString();
            }
        }
        timezone->millerTranstion();
        m_timezones.append(timezone);
    }
    db.commit();
    db.close();
}

MapWidget::MapWidget(QWidget *parent)
{
    m_map = QPixmap("/home/xingkd/Desktop/map_light.png");
    setFixedSize(m_map.size());
    setMouseTracking(true);
}

void MapWidget::setTimezones(QList<Timezone *> timezones)
{
    m_timezones = timezones;
}

void MapWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);

    painter.drawPixmap(0,0,m_map);

    QList<Timezone*>::iterator it = m_timezones.begin();
    painter.setBrush(Qt::red);
    for(; it != m_timezones.end(); it++)
    {
        QRect rect(0, 0, 5, 5);
        QPoint center(this->size().width()/2*(*it)->m_x, this->size().height()/2*(*it)->m_y);
        rect.moveCenter(center);
        painter.drawRect(rect);
    }

}

void MapWidget::mousePressEvent(QMouseEvent *e)
{

}

void MapWidget::mouseMoveEvent(QMouseEvent *e)
{
    QString str = QString("x:%1 , y:%2").arg(e->pos().x()).arg(e->pos().y());
    setToolTip(str);
}

void Timezone::millerTranstion()
{
    double PI = 3.141592653589793;
    double y = m_lon / 180;
    double rlat = m_lat * PI / 180;
    double x = 1.25 * log( tan( 0.25 * PI + 0.4 * rlat ) );
    x = x / 2.3034125433763912;

    m_x =  (y + 1);
    m_y =  (1 - x);
}
