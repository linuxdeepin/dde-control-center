#ifndef DISPLAYMODULE_H
#define DISPLAYMODULE_H

#include "moduleinterface.h"

class Monitor;
class DisplayModel;
class DisplayWorker;
class ModuleWidget;

namespace dcc {

namespace display {

class DisplayWidget;
class DisplayModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit DisplayModule(FrameProxyInterface *frame, QObject *parent = 0);
    ~DisplayModule();

private slots:
    void showResolutionDetailPage();
    void showCustomSettings();
    void showRecognize();
    void showRotate(Monitor *mon = nullptr);

private:
    void initialize();
    void reset();
    void moduleActive();
    void moduleDeactive();
    void contentPopped(ContentWidget * const w);
    const QString name() const;
    ModuleWidget *moduleWidget();

private:
    DisplayModel *m_displayModel;
    DisplayWorker *m_displayWorker;
    DisplayWidget *m_displayWidget;
};

} // namespace display

} // namespace dcc

#endif // DISPLAYMODULE_H
