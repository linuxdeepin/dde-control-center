#ifndef RINGCOLORWIDGET_H
#define RINGCOLORWIDGET_H

#include "roundcolorwidget.h"

#include <QWidget>

class RingColorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RingColorWidget(QWidget *parent = nullptr);
    virtual ~RingColorWidget() { }

    void setSelectedItem(RoundColorWidget *item)
    {
        m_selectedItem = item;
    }

    static const int EXTRA = 2; // 2px extra space to avoid line cutted off

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    RoundColorWidget *m_selectedItem;
};

#endif // RINGCOLORWIDGET_H
