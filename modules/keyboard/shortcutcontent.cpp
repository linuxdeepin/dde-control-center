#include "shortcutcontent.h"

#include "keyboardcontrol.h"
#include <QVBoxLayout>

ShortcutContent::ShortcutContent(QWidget *parent)
    :ContentWidget(parent)
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);

    KeyboardControl* control = new KeyboardControl();
    layout->addWidget(control);

    widget->setLayout(layout);
    setContent(widget);
}
