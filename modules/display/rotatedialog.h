#ifndef ROTATEDIALOG_H
#define ROTATEDIALOG_H

#include <QDialog>

#include "displayworker.h"

class RotateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RotateDialog(MonitorInter *inter, QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);

private:
    void rotate();

private:
    MonitorInter *m_displayInter;
};

#endif // ROTATEDIALOG_H
