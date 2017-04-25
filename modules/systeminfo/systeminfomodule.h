#ifndef SYSTEMINFOMODULE_H
#define SYSTEMINFOMODULE_H

#include "moduleinterface.h"
#include "systeminfowork.h"
#include "systeminfomodel.h"
#include "systeminfowidget.h"

#include <QObject>

namespace dcc{
namespace systeminfo{

class CopyrightWidget;
class BootWidget;

class SystemInfoModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit SystemInfoModule(FrameProxyInterface* frame, QObject* parent = 0);
    ~SystemInfoModule();

    void initialize();
    void moduleActive();
    void moduleDeactive();
    void reset();
    ModuleWidget *moduleWidget();

    const QString name() const;

    void contentPopped(ContentWidget * const w);

public slots:
    void onPushCopyright();
#ifndef DCC_DISABLE_GRUB
    void onPushBoot();
#endif

private:
    SystemInfoWork* m_work;
    SystemInfoModel* m_model;
#ifndef DCC_DISABLE_GRUB
    BootWidget* m_bootWidget;
#endif
    SystemInfoWidget* m_mainWidget;
    CopyrightWidget* m_copyrightWidget;
};

}
}
#endif // SYSTEMINFOMODULE_H
