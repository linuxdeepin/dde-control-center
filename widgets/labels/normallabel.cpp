#include "normallabel.h"

NormalLabel::NormalLabel(QWidget *parent)
    : QLabel(parent)
{

}

NormalLabel::NormalLabel(const QString &text, QWidget *parent)
    : QLabel(text, parent)
{

}
