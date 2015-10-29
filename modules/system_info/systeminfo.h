#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include <QObject>
#include <QFrame>
#include <QtPlugin>
#include <QScrollArea>

#include <libdui/dexpandgroup.h>
#include <libdui/dboxwidget.h>

#include "interfaces.h"
#include "dbus/dbussysteminfo.h"
#include "updatearrowexpand.h"
#include "updatewidget.h"
#include "mirrorscontrolwidget.h"

DUI_USE_NAMESPACE

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
    Q_SLOT void updateWidgetHeight();
    bool eventFilter(QObject *, QEvent *e) Q_DECL_OVERRIDE;

private:
    QFrame *m_centeralFrame;
    DBusSystemInfo m_dbusSystemInfo;
    QWidget *m_infoWidget;
    QScrollArea *m_licenseArea;
    ModuleHeader *m_baseLine;
    QWidget *m_updateWidget;
//    DVBoxWidget *m_updateWidget;
    UpdateWidget *m_updateInfoWidget;
    MirrorsControlWidget *m_mirrorsControlWidget;
    UpdateArrowExpand *m_updateExpand;
};

#endif // SYSTEMINFO_H
