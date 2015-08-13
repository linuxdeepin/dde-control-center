#include "headerline.h"

#include "highlightlabel.h"

HeaderLine::HeaderLine(QString title, QWidget *parent)
    : DBaseLine(parent)
{
    HighlightLabel * _title = new HighlightLabel(title, this);

    this->setLeftContent(_title);
}
