#ifndef WACOMWORKER_H
#define WACOMWORKER_H

#include "wacommodel.h"


#include <com_deepin_daemon_inputdevice_wacom.h>

#include "model/wacommodelbase.h"

#include <QObject>
using com::deepin::daemon::inputdevice::Wacom;

class WacomWorker : public QObject
{
    Q_OBJECT
public:
    explicit WacomWorker(WacomModel *model, QObject *parent = 0);
    void active();
    void deactive();

public slots:
    void setPressureSensitive(const int value);
    void onPressureSensitiveChanged(const int value);

private:
    Wacom *m_dbusWacom;
    WacomModel *m_model;
};

#endif // WACOMWORKER_H
