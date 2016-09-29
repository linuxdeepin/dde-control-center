#ifndef FRAME_H
#define FRAME_H

#include "mainwidget.h"

#include <QMainWindow>

class Frame : public QMainWindow
{
    Q_OBJECT

public:
    explicit Frame(QWidget *parent = 0);

private slots:
    void init();

private:
    MainWidget *m_mainWidget;
};

#endif // FRAME_H
