#ifndef CONNECTTOHIDDENAPWIDGET_H
#define CONNECTTOHIDDENAPWIDGET_H

#include <libdui/dboxwidget.h>
#include <libdui/libdui_global.h>

#include "dbus/dbusconnectionsession.h"

DUI_USE_NAMESPACE

class NetworkBaseEditLine;
class ConnectToHiddenApWidget : public DVBoxWidget
{
    Q_OBJECT
public:
    explicit ConnectToHiddenApWidget(DBusConnectionSession *dbus, QWidget *parent = 0);

signals:
    void cancel();
    void confirm();

private:
    void init();

    DBusConnectionSession *m_dbusConnectionSession;
    NetworkBaseEditLine *getLineEdit(const QString &section, const QString &key,
                                     const QString &title, bool isPasswordEdit = false);
    NetworkBaseEditLine *getCombobox(const QString &section, const QString &key, const QString &title);
    NetworkBaseEditLine *getFileChooser(const QString &section, const QString &key, const QString &title);
};

#endif // CONNECTTOHIDDENAPWIDGET_H
