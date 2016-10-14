#ifndef CLOCK_H
#define CLOCK_H

#include <QWidget>
#include <QPoint>
#include <QTimer>

class Clock:public QWidget
{
    Q_OBJECT

    public:
        /** Default constructor */
        Clock(QWidget *parent = 0);
        /** Default destructor */
        virtual ~Clock();
    protected:
        void paintEvent(QPaintEvent *event);
    private:
        QTimer *m_timer;

};
#endif // CLOCK_H
