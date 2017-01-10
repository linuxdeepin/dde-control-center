#ifndef ROTATEDIALOG_H
#define ROTATEDIALOG_H

#include <QDialog>

namespace dcc {

namespace display {

class Monitor;
class DisplayModel;
class RotateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RotateDialog(Monitor *mon, QWidget *parent = 0);
    explicit RotateDialog(DisplayModel *model, QWidget *parent = 0);
    ~RotateDialog();

signals:
    void requestRotate(Monitor *mon, const quint16 rotate) const;

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void resizeEvent(QResizeEvent *e);
    void leaveEvent(QEvent *e);
    void paintEvent(QPaintEvent *e);
    void showEvent(QShowEvent *e);

private:
    void init();
    void rotate();

private:
    Monitor *m_mon;
    DisplayModel *m_model;
};

} // namespace display

} // namespace dcc

#endif // ROTATEDIALOG_H
