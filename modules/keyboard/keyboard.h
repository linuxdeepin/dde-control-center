#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QObject>
#include <QtPlugin>

#include <libdui/libdui_global.h>
#include <libdui/dbuttonlist.h>

#include "interfaces.h"

#include "dbus/dbuskeyboard.h"

DUI_USE_NAMESPACE

class QFrame;
class SearchItem;
class SearchList;
class AddRmDoneLine;
class Keyboard: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "keyboard.json")
    Q_INTERFACES(ModuleInterface)

public:
    Keyboard();
    ~Keyboard() Q_DECL_OVERRIDE;
    QFrame* getContent() Q_DECL_OVERRIDE;

private:
    QFrame * m_frame;
    DBusKeyboard * m_dbusKeyboard;
    QMap<QString, QString> m_mapUserLayoutInfo;
    QMap<QString, int> m_mapUserLayoutIndex;
    QList<SearchItem*> m_selectLayoutList;

    void updateKeyboardLayout(SearchList *button_list, AddRmDoneLine *line, bool showRemoveButton = false);
    void initBackend();
    void initUI();
};

#endif //   KEYBOARD_H
