#ifndef EDITLINEMISSINGPACKAGE_H
#define EDITLINEMISSINGPACKAGE_H

#include "networkbaseeditline.h"

class EditLineMissingPackage : public NetworkBaseEditLine
{
    Q_OBJECT

public:
    explicit EditLineMissingPackage(const QString &section, const QString &key,
                              DBusConnectionSession *dbus, const QString &title,
                              QWidget *parent = 0);

    void setCurrentVpnName(const QString &name);

    QSize sizeHint() const Q_DECL_OVERRIDE;

signals:
    void visibleChanged(bool visible);

protected:
    void hideEvent(QHideEvent *e) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent *e) Q_DECL_OVERRIDE;

private:
    bool m_isVisible = false;
};

#endif // EDITLINEMISSINGPACKAGE_H
