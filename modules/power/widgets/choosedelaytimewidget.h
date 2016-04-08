#ifndef CHOOSEDELAYTIMEWIDGET_H
#define CHOOSEDELAYTIMEWIDGET_H

#include <QWidget>

#include <dbuttongrid.h>

#include "doublecornerwidget.h"

DWIDGET_USE_NAMESPACE
class ChooseDelayTimeWidget:public DoubleCornerWidget {
    Q_OBJECT
public:
    ChooseDelayTimeWidget(QString title, QWidget* parent=0);
    ~ChooseDelayTimeWidget();
signals:
    void currentTimeChanged(int time);
public slots:
    void setCurrentTime(int time);
    void handleCurrentItemIndexChanged(int index);

private:
    QLabel* m_titleLabel;
    QLabel* m_numLabel;
    QString m_currentNum = "10m";
    DButtonGrid* m_timeBtnGrid;

};

DWIDGET_USE_NAMESPACE
#endif // CHOOSEDELAYTIMEWIDGET_H
