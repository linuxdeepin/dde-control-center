#ifndef DATETIMEWIDGET_H
#define DATETIMEWIDGET_H

#include <QFrame>
#include <QLineEdit>

class DateWidget : public QFrame
{
    Q_OBJECT

public:
    enum Type{
        Year,
        Month,
        Day
    };

public:
    explicit DateWidget(Type type, QWidget *parent = 0);
    QString dateString() const;
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

private:
    QLineEdit *m_lineEdit;
    QRect m_plus;
    QRect m_sub;
    QRect m_text;
    Type m_type;
    QString m_unit;
    QString m_dateText;

    int m_max;
};

class TimeWidget : public QFrame
{
    Q_OBJECT

public:
    explicit TimeWidget(QWidget* parent = 0);

public slots:
    void setHourText();
    void setMinuteText();

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);

private:
    QLineEdit *m_hourEdit;
    QLineEdit *m_minuteEdit;
    QRect m_hour;
    QRect m_minute;

    QString m_hourText;
    QString m_minuteText;

    QFont m_font;
};


#endif // DATETIMEWIDGET_H
