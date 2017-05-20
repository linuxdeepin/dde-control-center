#ifndef DISPLAYMODULE_H
#define DISPLAYMODULE_H

#include "moduleinterface.h"


namespace dcc {
class ModuleWidget;

namespace display {
class Monitor;
class DisplayWorker;
class DisplayModel;
class DisplayWidget;
class DisplayModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit DisplayModule(FrameProxyInterface *frame, QObject *parent = 0);
    ~DisplayModule();

private slots:
    void showBrightnessPage();
    void showResolutionDetailPage();
    void showCustomSettings(const QString &config);
    void showConfigPage(const QString &config);
    void showRecognize();
#ifndef DCC_DISABLE_ROTATE
    void showRotate(Monitor *mon = nullptr);
#endif

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
