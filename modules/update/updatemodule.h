#ifndef UPDATEMODULE_H
#define UPDATEMODULE_H

#include "moduleinterface.h"
#include "updatectrlwidget.h"
#include "updatesettings.h"
#include "mirrorswidget.h"
#include "updatemodel.h"
#include "updatework.h"

#include <QObject>

class UpdateView;
class UpdateModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit UpdateModule(FrameProxyInterface *frame, QObject *parent = 0);

private:
    ~UpdateModule();
    void initialize();
    void moduleActive();
    void moduleDeactive();
    void reset();
    void contentPopped(ContentWidget * const w);
    ModuleWidget* moduleWidget();
    const QString name() const;

private slots:
    void onPushUpdate();
    void onPushMirror();
    void onPushSettings();

    void setCurMirrorName(const QString& name, const QString &src);
    void setAutoUpdate(bool autoUpdate);

    void onActionType(UpdateType type);

private:
    UpdateWork* m_work;
    UpdateModel* m_model;
    UpdateView* m_updateView;
    UpdateCtrlWidget* m_updatePage;
    UpdateSettings* m_updateMirrors;
    MirrorsWidget* m_mirrorsWidget;
};

#endif // UPDATEMODULE_H
