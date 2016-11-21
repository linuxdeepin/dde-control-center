#ifndef UPDATEMODULE_H
#define UPDATEMODULE_H

#include "moduleinterface.h"
#include "updatectrlwidget.h"

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
    void showUpdatePage();

private:
    UpdateView* m_updateView;

    UpdateCtrlWidget* m_updatePage;
};

#endif // UPDATEMODULE_H
