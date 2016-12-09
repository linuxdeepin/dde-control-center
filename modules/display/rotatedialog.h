#ifndef ROTATEDIALOG_H
#define ROTATEDIALOG_H

#include <QDialog>

namespace dcc {

namespace display {

class Monitor;
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

} // namespace display

} // namespace dcc

#endif // ROTATEDIALOG_H
