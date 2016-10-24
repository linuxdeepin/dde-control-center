#ifndef CLOCK_H
#define CLOCK_H

#include <QFrame>
#include <QPoint>
#include <QTimer>
#include <QTimeZone>
#include "settingsitem.h"
using namespace dcc;

class Clock: public SettingsItem
{
    Q_OBJECT

public:
    explicit Clock(QFrame *parent = 0);
    virtual ~Clock();

    void setTimezone(const QString& timezone);
    void setDisplay(bool display);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QTimer *m_timer;
    QTimeZone m_timezone;
    bool m_display;
};
#endif // CLOCK_H
