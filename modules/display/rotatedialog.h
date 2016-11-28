#ifndef ROTATEDIALOG_H
#define ROTATEDIALOG_H

#include <QDialog>

#include "displayworker.h"
#include "monitor.h"

class RotateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RotateDialog(Monitor *mon, QWidget *parent = 0);

signals:
    void requestRotate(Monitor *mon, const quint16 rotate) const;

protected:
    void mousePressEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);

private:
    void rotate();

private:
    Monitor *m_mon;
};

#endif // ROTATEDIALOG_H
