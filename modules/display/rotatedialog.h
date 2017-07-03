#ifndef ROTATEDIALOG_H
#define ROTATEDIALOG_H

#include <QDialog>
#include <QTimer>

#include "DWindowManagerHelper"

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
    void requestRotateAll(const quint16 rotate) const;
    void requestRotate(Monitor *mon, const quint16 rotate) const;

protected:
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void leaveEvent(QEvent *e);
    void paintEvent(QPaintEvent *e);
    void showEvent(QShowEvent *e);

private slots:
    void adjustGemotry();

private:
    void init();
    void rotate();

private:
    Monitor *m_mon;
    DisplayModel *m_model;
    QTimer *m_adjustDelayTimer;
    Dtk::Widget::DWindowManagerHelper *m_wmHelper;

    bool m_mouseLeftHand;
};

} // namespace display

} // namespace dcc

#endif // ROTATEDIALOG_H
