#ifndef DATETIME_H
#define DATETIME_H

#include "datesettings.h"
#include "contentwidget.h"

#include <modulewidget.h>
#include <settingsgroup.h>
#include <settingsitem.h>

using namespace dcc;

class Datetime : public ModuleWidget
{
    Q_OBJECT

public:
    explicit Datetime();
    void addTimezone(const QString& city);

public slots:
    void slotClick();

private:
    SettingsGroup* m_group;
    ContentWidget* m_dateSettings;
};

#endif // DATETIME_H
