#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include "contentwidget.h"
#include "moduleinterface.h"

#include <QMap>
#include <QVBoxLayout>

class Frame;
class SettingsWidget : public ContentWidget, public FrameProxyInterface
{
    Q_OBJECT

public:
    explicit SettingsWidget(Frame *frame);

signals:
    void requestAutohide(const bool autoHide) const;

public slots:
    void contentPopuped(ContentWidget * const w);

    void setFrameAutoHide(ModuleInterface * const inter, const bool autoHide);
    void pushWidget(ModuleInterface * const inter, ContentWidget * const w);

private slots:
    void loadModule(ModuleInterface * const module);
    void onModuleInitFinished(ModuleInterface * const inter);
    void refershModuleActivable();

private:
    ~SettingsWidget();

private:
    Frame *m_frame;

    QVBoxLayout *m_settingsLayout;
    QWidget *m_settingsWidget;

    QList<ModuleInterface *> m_moduleInterfaces;
    QMap<ModuleInterface *, bool> m_moduleActivable;
    QMap<ModuleInterface *, QList<ContentWidget *>> m_moduleWidgets;

    QTimer *m_refershModuleActivableTimer;
};

#endif // SETTINGSWIDGET_H
