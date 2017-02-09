#ifndef WEATHERWIDGET_H
#define WEATHERWIDGET_H

#include <QWidget>
#include "weatherrequest.h"
#include <QTimer>
#include <QResizeEvent>
#include <dpicturesequenceview.h>
DWIDGET_USE_NAMESPACE

class WeatherWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WeatherWidget(QWidget* parent = 0);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent* e);
    QString icon(const WeatherItem& item);

public slots:
    void refreshView(QList<WeatherItem> &items);

private:
    QTime m_time;
    QTimer* m_timer;
    WeatherRequest *m_request;
    QList<QPixmap> m_pixmaps;
    DPictureSequenceView* m_view;
};

#endif // WEATHERWIDGET_H
