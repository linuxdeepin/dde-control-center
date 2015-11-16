#ifndef BUTTONGROUP_H
#define BUTTONGROUP_H

#include <QFrame>

class ButtonGroup : public QWidget
{
    Q_OBJECT
public:
    ButtonGroup(const QString& title);

    ButtonGroup* addItem(QWidget*);
    ButtonGroup* addSeparator();

private:
};

#endif // BUTTONGROUP_H
