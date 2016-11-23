#include "largelabel.h"

LargeLabel::LargeLabel(QWidget *parent)
    : QLabel(parent)
{

}

LargeLabel::LargeLabel(const QString &text, QWidget *parent)
    : QLabel(text, parent)
{

}
