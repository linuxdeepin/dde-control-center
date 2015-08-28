#ifndef TIMEZONEWIDGET_H
#define TIMEZONEWIDGET_H

#include <QFrame>
#include <QLabel>

#include "multideletebutton.h"
#include "multiaddcheckbutton.h"

#include <libdui/dimagebutton.h>

DUI_USE_NAMESPACE

class TimezoneWidget : public QFrame
{
    Q_OBJECT
public:
    enum EchoState {SelectOrRemove, SelectableOnly};

public:
    explicit TimezoneWidget(QWidget *parent = 0);

signals:
    void clicked();

public slots:
    inline void setZoneCities(const QString & cities) {m_citiesLabel->setText(cities);}
    inline void setZoneUTCOffset(const QString & UTCOffset) {m_utcOffsetLabel->setText(UTCOffset);}

private:
    QLabel *m_citiesLabel;
    QLabel *m_utcOffsetLabel;
    MultiDeleteButton *m_removeBtn;
    DImageButton *m_selectedBtn;
    MultiAddCheckButton *m_chooseBtn;
};

#endif // TIMEZONEWIDGET_H
