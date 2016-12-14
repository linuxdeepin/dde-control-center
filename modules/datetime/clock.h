#ifndef CLOCK_H
#define CLOCK_H

#include <QFrame>
#include <QPoint>
#include <QTimer>
#include <QTimeZone>
#include "settingsitem.h"
using namespace dcc;

namespace dcc {
namespace datetime {

class Clock: public widgets::SettingsItem
{
    Q_OBJECT

    Q_PROPERTY(bool display READ display WRITE setDisplay  DESIGNABLE true SCRIPTABLE true)

public:
    explicit Clock(QFrame *parent = 0);
    virtual ~Clock();

    void setTimezone(const QString& timezone);
    void setDisplay(bool display);
    bool display() const { return m_display;}

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *e);

private:
    QTimer *m_timer;
    QTimeZone m_timezone;
    bool m_display;
};
}
}
#endif // CLOCK_H
