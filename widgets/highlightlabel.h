#ifndef HIGHLIGHTLABEL_H
#define HIGHLIGHTLABEL_H

#include <QLabel>

class HighlightLabel : public QLabel
{
    Q_OBJECT
public:
    explicit HighlightLabel(QWidget *parent = 0);
    HighlightLabel(QString text, QWidget *parent = 0);
};

#endif // HIGHLIGHTLABEL_H
