#ifndef NORMALLABEL_H
#define NORMALLABEL_H

#include <QLabel>

class NormalLabel : public QLabel
{
    Q_OBJECT
public:
    explicit NormalLabel(QWidget *parent = 0);
    NormalLabel(QString title, QWidget *parent = 0);
};

#endif // NORMALLABEL_H
