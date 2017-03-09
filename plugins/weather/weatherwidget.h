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

public slots:
    void refreshView(QList<WeatherItem> &items);

private:
    QTime m_time;
    QTimer* m_timer;
    WeatherRequest *m_request;
    QList<QPixmap> m_pixmaps;
    DPictureSequenceView* m_view;
    DImageButton *m_locationBtn;
};

#endif // WEATHERWIDGET_H
