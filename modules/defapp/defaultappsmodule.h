#ifndef DEFAULTAPPSMODULE_H
#define DEFAULTAPPSMODULE_H

#include "moduleinterface.h"

namespace dcc {
namespace defapp {
class DefAppModel;
class DefAppWorker;
class DefaultAppsWidget;
class DefAppViewer;
class DefaultAppsModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit DefaultAppsModule(FrameProxyInterface *frame, QObject *parent = 0);
    ~DefaultAppsModule();

    void initialize();
    void moduleActive();
    void moduleDeactive();
    void reset();

    ModuleWidget *moduleWidget();

    const QString name() const;

signals:
    void requestReset();

private:
    void showDefaultAppsDetail();
    void contentPopped(ContentWidget * const w);
    void setFrameAutoHide(const bool autoHide);

private:
    DefAppModel       *m_defAppModel;
    DefAppWorker      *m_defAppWorker;
    DefaultAppsWidget *m_defaultappsWidget;
    DefAppViewer      *m_defaultAppsDetail;
};
}
}
#endif // DEFAULTAPPSMODULE_H
