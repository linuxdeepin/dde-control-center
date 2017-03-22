#ifndef UPDATEMODULE_H
#define UPDATEMODULE_H

#include "moduleinterface.h"
#include "updatectrlwidget.h"
#include "updatesettings.h"
#include "mirrorswidget.h"
#include "updatemodel.h"
#include "updatework.h"

#include <QObject>

namespace dcc{
namespace update{

class UpdateView;

class UpdateModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit UpdateModule(FrameProxyInterface *frame, QObject *parent = 0);
    ~UpdateModule();

private:
    void initialize();
    void moduleActive();
    void moduleDeactive();
    void reset();
    void contentPopped(ContentWidget * const w);
    ModuleWidget* moduleWidget();
    const QString name() const;
    void showPage(const QString &pageName);

private slots:
    void onPushUpdate();
    void onPushMirrorsView();
    void onPushSettings();

private:
    UpdateWork* m_work;
    UpdateModel* m_model;
    UpdateView* m_updateView;
    UpdateCtrlWidget* m_updatePage;
    UpdateSettings* m_settingsPage;
    MirrorsWidget* m_mirrorsWidget;
};

}
}
#endif // UPDATEMODULE_H
