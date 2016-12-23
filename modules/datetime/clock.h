#ifndef CLOCK_H
#define CLOCK_H

#include <QWidget>
#include <QTimeZone>

namespace dcc {
namespace datetime {

class Clock: public QWidget
{
    Q_OBJECT
public:
    explicit Clock(QWidget *parent = 0);
    virtual ~Clock();

    bool drawTicks() const;
    void setDrawTicks(bool drawTicks);

    QTimeZone timeZone() const;
    void setTimeZone(const QTimeZone &timeZone);

protected:
    void paintEvent(QPaintEvent *event);

private:
    bool m_drawTicks;
    QTimeZone m_timeZone;
};
}
}
#endif // CLOCK_H
