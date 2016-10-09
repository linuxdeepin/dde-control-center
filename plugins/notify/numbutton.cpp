#include "numbutton.h"

NumButton::NumButton(QWidget *parent){
    Q_UNUSED(parent);

}

NumButton::NumButton(const QString &text, QWidget *parent, Qt::WindowFlags f):QLabel(text,parent,f) {

}

void NumButton::mouseReleaseEvent(QMouseEvent *ev) {
     Q_UNUSED(ev)
    emit clicked();
}
