#ifndef QUICKSWITCHBUTTON_H
#define QUICKSWITCHBUTTON_H

#include <QLabel>

class QuickSwitchButton : public QLabel
{
    Q_OBJECT
public:
    explicit QuickSwitchButton(const int index, QWidget *parent = 0);

signals:
    void hovered(const int index) const;
    void clicked(const int index) const;

protected:
    void mouseReleaseEvent(QMouseEvent *e);

private:
    const int m_index;
};

#endif // QUICKSWITCHBUTTON_H
