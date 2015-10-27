#ifndef UPDATEWIDGET_H
#define UPDATEWIDGET_H

#include <QWidget>
#include <QLabel>

#include <libdui/dcircleprogress.h>

DUI_USE_NAMESPACE

class UpdateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UpdateWidget(QWidget *parent = 0);

private:
    void loadAppList();

private:
    QLabel *m_updateCountTips;
    QLabel *m_updateSizeTips;
    DCircleProgress *m_updateProgress;
};

#endif // UPDATEWIDGET_H
