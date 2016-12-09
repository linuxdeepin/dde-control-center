#ifndef DATETIMEWIDGET_H
#define DATETIMEWIDGET_H

#include <QFrame>
#include <QLineEdit>
#include <dimagebutton.h>
#include "settingsitem.h"

DWIDGET_USE_NAMESPACE
using namespace dcc;

namespace dcc {
namespace datetime {

class DateWidget : public SettingsItem
{
    Q_OBJECT

public:
    enum Type{
        Year,
        Month,
        Day
    };

public:
    explicit DateWidget(Type type, QFrame *parent = 0);
    int data() const;
    void setMax(int max);
    int max() const;

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);

signals:
    void dataChanged(DateWidget::Type type, int data);

public slots:
    void setText();
    void slotAdd();
    void slotReduced();

private:
    QLineEdit *m_lineEdit;
    QRect m_plus;
    QRect m_sub;
    QRect m_text;
    Type m_type;
    QString m_unit;
    QString m_dateText;
    DImageButton* m_addBtn;
    DImageButton* m_reducedBtn;
    int m_max;
};

class TimeWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit TimeWidget(QFrame* parent = 0);
    int hour() const;
    int minute() const;

public slots:
    void setHourText();
    void setMinuteText();

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);
    void showEvent(QShowEvent *e);

private:
    QLineEdit *m_hourEdit;
    QLineEdit *m_minuteEdit;
    QRect m_hour;
    QRect m_minute;

    QString m_hourText;
    QString m_minuteText;

    QFont m_font;
};
}
}

#endif // DATETIMEWIDGET_H
