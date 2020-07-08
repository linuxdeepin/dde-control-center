#ifndef UNIONIDWORKER_H
#define UNIONIDWORKER_H

#include "modules/moduleworker.h"
#include "unionidmodel.h"
#include "window/mainwindow.h"

#include <QObject>
#include <com_deepin_sync_daemon.h>
#include <com_deepin_deepinid.h>

using SyncInter = com::deepin::sync::Daemon;
using DeepinId = com::deepin::deepinid;

namespace dcc {
namespace unionid {
class UnionidWorker : public QObject, public ModuleWorker
{
    Q_OBJECT
public:
    explicit UnionidWorker(UnionidModel * model, QObject *parent = nullptr);

    virtual void activate();
    virtual void deactivate();


public Q_SLOTS:
    void loginUser();
    void logoutUser();
    void setAutoSync(bool autoSync);
    void requestAgreementPopup(const QString &fileName);
    void  requestModifyDialog(QString dilogName);
private:
    void onStateChanged(const IntString& state);

private:
    UnionidModel *m_model;
    SyncInter *m_syncInter;
    DeepinId *m_deepinId_inter;
};
}
}

#endif // SYNCWORKER_H
