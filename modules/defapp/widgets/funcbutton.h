#ifndef FUNCBUTTON_H
#define FUNCBUTTON_H

#include <QWidget>
#include <QLabel>
#include <QEvent>
#include <QMouseEvent>
namespace dcc
{
namespace defapp
{
class FuncButton : public QLabel
{
    Q_OBJECT
public:
    explicit FuncButton(QWidget *parent =0);

signals:
    void clicked();

protected:
    void enterEvent(QEvent * event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent * event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent * event) Q_DECL_OVERRIDE;

};
}
}


#endif // FUNCBUTTON_H
