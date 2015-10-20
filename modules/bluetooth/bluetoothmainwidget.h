#ifndef BLUETOOTHMAINWIDGET_H
#define BLUETOOTHMAINWIDGET_H

#include <QFrame>
#include <QVBoxLayout>

class ListWidget;
class BluetoothMainWidget : public QFrame
{
    Q_OBJECT
public:
    explicit BluetoothMainWidget(QWidget *parent = 0);

protected:
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

private:
    QVBoxLayout *m_mainLayout;
    ListWidget *m_deviceList;

    void initUI();
    void updateUI();
};

#endif // BLUETOOTHMAINWIDGET_H
