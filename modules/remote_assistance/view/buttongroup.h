#ifndef BUTTONGROUP_H
#define BUTTONGROUP_H

#include <QFrame>

#include <libdui/libdui_global.h>

DUI_BEGIN_NAMESPACE
class DHeaderLine;
DUI_END_NAMESPACE

class ButtonGroup : public QWidget
{
    Q_OBJECT
public:
    ButtonGroup();
    ButtonGroup(const QString& title);

    ButtonGroup* addItem(QWidget*);
    ButtonGroup* addSeparator();
    ButtonGroup* setGroupTitle(const QString& title);

private:
    DUI_NAMESPACE::DHeaderLine* m_header;
};

#endif // BUTTONGROUP_H
