#ifndef WEATHERWIDGET_H
#define WEATHERWIDGET_H

#include <QWidget>
#include "weatherrequest.h"

class WeatherWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WeatherWidget(QWidget* parent = 0);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    QString icon(const WeatherItem& item);

public slots:
    void refreshView(QList<WeatherItem> &items);

private:
    WeatherRequest *m_request;
    QMap<QString, QString> m_iconsDict;
};

#endif // WEATHERWIDGET_H
