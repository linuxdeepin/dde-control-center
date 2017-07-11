#ifndef INDICATORWIDGET_H
#define INDICATORWIDGET_H

#include <QWidget>

#include <dpageindicator.h>
#include <dimagebutton.h>

class IndicatorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IndicatorWidget(QWidget *parent = 0);

signals:
    void requestPrevious();
    void requestNext();

private:
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void wheelEvent(QWheelEvent *e);

private:
    Dtk::Widget::DPageIndicator *m_pluginsIndicator;
    Dtk::Widget::DImageButton *m_prevPluginBtn;
    Dtk::Widget::DImageButton *m_nextPluginBtn;

    QTimer *m_ignoreWheelRepeat;
};

#endif // INDICATORWIDGET_H
