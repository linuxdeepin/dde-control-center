#ifndef RECOGNIZEDIALOG_H
#define RECOGNIZEDIALOG_H

#include <QDialog>

class Monitor;
class DisplayModel;
class RecognizeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RecognizeDialog(DisplayModel *model, QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *);

private slots:
    void onScreenRectChanged();

private:
    void paintMonitorMark(QPainter &painter, const QRect &rect, const QString &name);

private:
    DisplayModel *m_model;
};

#endif // RECOGNIZEDIALOG_H
