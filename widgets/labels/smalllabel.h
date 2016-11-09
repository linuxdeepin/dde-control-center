#ifndef SMALLLABEL_H
#define SMALLLABEL_H

#include <QLabel>

class SmallLabel : public QLabel
{
    Q_OBJECT

public:
    explicit SmallLabel(QWidget *parent = 0);
};

#endif // SMALLLABEL_H
