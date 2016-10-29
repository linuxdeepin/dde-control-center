#ifndef CHOSEDIALOG_H
#define CHOSEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

struct Timezone
{
    double m_lon;
    double m_lat;
    double m_x;
    double m_y;
    QString m_city;
    QString m_timezone;

    void millerTranstion();

};


class MapWidget;
class ChoseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChoseDialog(QWidget* parent = 0);

    void initData();

private:
    MapWidget* m_widget;
    QLineEdit* m_search;
    QPushButton* m_cancel;
    QPushButton* m_add;
    QList<Timezone*> m_timezones;
};


class MapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MapWidget(QWidget* parent = 0);
    void setTimezones(QList<Timezone*> timezones);

protected:
    void paintEvent(QPaintEvent* e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

private:
    QPixmap m_map;
    QList<Timezone*> m_timezones;
};

#endif // CHOSEDIALOG_H
