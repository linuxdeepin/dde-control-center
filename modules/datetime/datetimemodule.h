#ifndef DATETIMEMODULE_H
#define DATETIMEMODULE_H

#include "moduleinterface.h"
#include "datetime.h"
#include "datetimework.h"
#include "datetimemodel.h"

class DatetimeModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit DatetimeModule(FrameProxyInterface* frame, QObject* parent = 0);

    void initialize();
    void moduleActive();
    void moduleDeactive();

    ModuleWidget *moduleWidget();

    const QString name() const;

    void contentPopped(ContentWidget * const w);

public slots:
    void slotEditDatetime();

private:
    Datetime* m_datetimeWidget;
    DateSettings* m_dateSettings;
    DatetimeWork* m_work;
    DatetimeModel* m_model;
};

#endif // DATETIMEMODULE_H
