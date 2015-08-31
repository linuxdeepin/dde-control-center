#ifndef TITLEANDWIDGET_H
#define TITLEANDWIDGET_H

#include <QLabel>

class TitleAndWidget : public QLabel
{
    Q_OBJECT
public:
    explicit TitleAndWidget(QWidget *w, const QString &title = "", QWidget *parent = 0);
};

#endif // TITLEANDWIDGET_H
