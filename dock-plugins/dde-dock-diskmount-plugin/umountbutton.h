#ifndef UMOUNTBUTTON_H
#define UMOUNTBUTTON_H

#include <QWidget>
#include <QLabel>
#include <QPixmap>

class UmountButton : public QLabel
{
    Q_OBJECT
public:
    explicit UmountButton(QWidget *parent = 0);

signals:
    void mouseHovered();
    void mouseExited();
    void mousePressed();

protected:
    void mousePressEvent(QMouseEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

public slots:
};

#endif // UMOUNTBUTTON_H
