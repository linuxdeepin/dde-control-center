#ifndef CLOCKPIXMAP_H
#define CLOCKPIXMAP_H

#include <QPixmap>
#include <QTime>
#include <QList>

class ClockPixmap : public QPixmap
{
public:
    ClockPixmap();
    ClockPixmap(QTime time);
    ~ClockPixmap();

    QTime getTime() const;
    void setTime(const QTime &time);

    bool getIn24hour() const;
    void setIn24hour(bool in24hour);

    bool getAnalog() const;
    void setAnalog(bool analog);

private:
    QTime m_time;
    bool m_in24hour;
    bool m_analog;

    void paint();
    QPixmap getNamedPixmap(QString name);
    QPixmap getNumberPixmap(int number, bool big=true);

    QList<int> getTimeTuple();
};

#endif // CLOCKPIXMAP_H
