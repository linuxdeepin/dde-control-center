#ifndef RECOGNIZEDIALOG_H
#define RECOGNIZEDIALOG_H

#include <QDialog>

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
    DisplayModel *m_model;
};

#endif // RECOGNIZEDIALOG_H
