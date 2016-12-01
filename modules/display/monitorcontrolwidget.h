#ifndef MONITORCONTROLWIDGET_H
#define MONITORCONTROLWIDGET_H

#include <QWidget>
#include <QPushButton>

class MonitorControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MonitorControlWidget(QWidget *parent = 0);

signals:
    void requestRecognize() const;

private:
    QPushButton *m_recognize;
    QPushButton *m_split;
    QPushButton *m_join;
};

#endif // MONITORCONTROLWIDGET_H
