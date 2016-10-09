#ifndef NUMBUTTON_H
#define NUMBUTTON_H

#include <QObject>
#include <QLabel>
#include <QtWidgets>
class NumButton : public QLabel
{
    Q_OBJECT
public:
    explicit NumButton(QWidget *parent = 0);
    explicit NumButton(const QString &text, QWidget *parent=0, Qt::WindowFlags f=0);
protected:
      virtual void mouseReleaseEvent(QMouseEvent * ev);
signals:
    void clicked();
public slots:
};

#endif // NUMBUTTON_H
