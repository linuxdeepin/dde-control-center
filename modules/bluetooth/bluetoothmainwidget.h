#ifndef BLUETOOTHMAINWIDGET_H
#define BLUETOOTHMAINWIDGET_H

#include <QFrame>
#include <QVBoxLayout>

class BluetoothMainWidget : public QFrame
{
    Q_OBJECT
public:
    explicit BluetoothMainWidget(QWidget *parent = 0);

private:
    QVBoxLayout *m_mainLayout;

    void initUI();
};

#endif // BLUETOOTHMAINWIDGET_H
