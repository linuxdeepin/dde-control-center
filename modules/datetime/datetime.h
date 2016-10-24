#ifndef DATETIME_H
#define DATETIME_H

#include <modulewidget.h>
#include "datesettings.h"
#include <settingsgroup.h>
#include <settingsitem.h>
using namespace dcc;

class Datetime : public ModuleWidget
{
    Q_OBJECT

public:
    explicit Datetime(FrameProxyInterface *frame);
    void addTimezone(const QString& city);

public slots:
    void slotClick();

private:
    SettingsGroup* m_group;
    ContentWidget* m_dateSettings;
};

#endif // DATETIME_H
