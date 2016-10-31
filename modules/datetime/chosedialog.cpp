#include "chosedialog.h"

#include <QMenu>
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
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>

ChoseDialog::ChoseDialog(QWidget *parent)
    :QDialog(parent)
{
    m_widget = new MapWidget();
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

    QCompleter *completer = new QCompleter(m_widget->citys());
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_search->setCompleter(completer);

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

    connect(m_cancel, SIGNAL(clicked()), this, SLOT(hide()));
    connect(m_add, SIGNAL(clicked()), this, SLOT(onAdd()));
    connect(m_search, SIGNAL(returnPressed()), this, SLOT(onReturn()));
    connect(m_widget, SIGNAL(cityChanged(QString)), m_search, SLOT(setText(QString)));
}

void ChoseDialog::onReturn()
{
    QString city = m_search->text().simplified();

    m_widget->onLocateCity(city);
}

void ChoseDialog::onAdd()
{
    emit addTimezone(m_widget->timezone());
    hide();
}

void MapWidget::initData()
{
    m_timezones.clear();
    QSqlDatabase db;
    if(QSqlDatabase::contains(timezone_database))
    {
        db = QSqlDatabase::database(QLatin1String(timezone_database));
    }
    else
    {
        db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), timezone_database);
    }
    db.setDatabaseName(CITIES_DATABASE_PATH + QString(timezone_database) + ".db");

    if (!db.open())
    {
        qWarning() << QString("Failed to open %1 database").arg(db.databaseName());
        qWarning() << db.lastError();
        return;
    }

    QSqlQuery query(db);

    QString schema = QString("SELECT timezone, lat, lon, name from tzcity");

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
            else if(field.name() == "lat")
            {
                timezone->m_lat = field.value().toDouble();
            }
            else if(field.name() == "lon")
            {
                timezone->m_lon = field.value().toDouble();
            }
            else
            {
                timezone->m_city = field.value().toString();
                if(!m_timezoneLists.contains(timezone->m_city))
                {
                    m_timezoneLists.append(timezone->m_city);
                }
            }
        }
        timezone->millerTranstion(m_map.size().width(), m_map.size().height());
        m_timezones.append(timezone);
    }
    db.commit();
    db.close();
}

MapWidget::MapWidget(QWidget *parent)
    :QFrame(parent),
      m_menu(new QMenu(this)),
      m_curTimezone(Timezone(false))
{
    m_map = QPixmap(":/icon/map.svg");
    setFixedSize(m_map.size());
    setStyleSheet("background-color: blue; opacity: 0.2;");

    initData();

    connect(m_menu, SIGNAL(triggered(QAction*)), this, SLOT(onChoseCity(QAction*)));
}

void MapWidget::setTimezones(QList<Timezone *> timezones)
{
    m_timezones = timezones;
}

QStringList MapWidget::citys() const
{
    return m_timezoneLists;
}

Timezone MapWidget::timezone() const
{
    return m_curTimezone;
}

void MapWidget::onLocateCity(const QString &city)
{
    QList<Timezone*>::iterator it = m_timezones.begin();
    for(; it != m_timezones.end(); it++)
    {
        if((*it)->m_city == city)
        {
            m_curTimezone = *(*it);
            break;
        }
    }
    update();
}

void MapWidget::onChoseCity(QAction *act)
{
    QJsonObject json = act->data().toJsonObject();
    m_curTimezone.m_city = json.value("name").toString();
    m_curTimezone.m_x = json.value("x").toDouble();
    m_curTimezone.m_y = json.value("y").toDouble();
    m_curTimezone.m_timezone = json.value("timezone").toString();
    m_curTimezone.m_valid = true;

    emit cityChanged(m_curTimezone.m_city);
    update();
}

void MapWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.drawPixmap(0,0,m_map);

    painter.setBrush(Qt::red);

    if(m_curTimezone.m_valid)
    {
        QRect rect(0, 0, 5, 5);
        rect.moveCenter(QPoint(m_curTimezone.m_x, m_curTimezone.m_y));
        painter.drawRect(rect);
    }
}

void MapWidget::mousePressEvent(QMouseEvent *e)
{
    m_menu->clear();
    QRect rect(0,0,5,5);
    rect.moveCenter(e->pos());
    QList<Timezone*>::iterator it = m_timezones.begin();

    int count = 0;
    for(; it != m_timezones.end(); it++)
    {
        QPoint pos = QPoint((*it)->m_x, (*it)->m_y);
        if(rect.contains(pos))
        {
            QAction* act = m_menu->addAction((*it)->m_city);
            count++;
            QJsonObject jobj{
                {"name",(*it)->m_city},
                {"x",(*it)->m_x},
                {"y",(*it)->m_y},
                {"timezone",(*it)->m_timezone},
            };
            act->setData(jobj);
            m_curTimezone = *(*it);
        }
    }
    if(count > 1)
    {
        m_curTimezone.m_valid = false;
        m_menu->exec(QCursor::pos());
    }
    else
    {
        emit cityChanged(m_curTimezone.m_city);
    }
    update();
}
