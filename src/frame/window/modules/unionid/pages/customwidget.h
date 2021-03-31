#ifndef CUSTOMWIDGET_H
#define CUSTOMWIDGET_H

#include <QWidget>

class CustomWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CustomWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event);

};

#endif // CUSTOMWIDGET_H
