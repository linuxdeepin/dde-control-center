#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include <QObject>
#include <QFrame>
#include <QtPlugin>
#include <QScrollArea>

#include "interfaces.h"
#include "dbus/dbussysteminfo.h"

class QFrame;
class QPlainTextEdit;
class ModuleHeader;
class SystemInfo : public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "system_info.json")
    Q_INTERFACES(ModuleInterface)

public:
    SystemInfo();
    ~SystemInfo() Q_DECL_OVERRIDE;
    QFrame* getContent() Q_DECL_OVERRIDE;

private:
    QString inline formatCap(qulonglong cap) const;
    QString inline getLicense(const QString & filePath, const QString &type) const;
    Q_SLOT void updateLicenseWidget();
    bool eventFilter(QObject *, QEvent *e);

private:
    QFrame *m_centeralFrame;
    DBusSystemInfo m_dbusSystemInfo;
    QWidget *m_infoWidget;
    QScrollArea *m_licenseArea;
    ModuleHeader *m_baseLine;
};

#endif // SYSTEMINFO_H
