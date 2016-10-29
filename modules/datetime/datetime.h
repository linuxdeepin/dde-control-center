#ifndef DATETIME_H
#define DATETIME_H

#include "datesettings.h"
#include "contentwidget.h"

#include <modulewidget.h>
#include <settingsgroup.h>
#include <settingsitem.h>
#include "timezonehead.h"
#include "nextpagewidget.h"

using namespace dcc;

class Datetime : public ModuleWidget
{
    Q_OBJECT

public:
    explicit Datetime();
    void addTimezone(const QString& city);

signals:
    void editChanged(bool edit);
    void editDatetime();

public slots:
    void slotClick();
    void slotEditMode(bool edit);

private:
    bool m_bEdit;
    TimezoneHead* m_headItem;
    NextPageWidget* m_addItem;
    SettingsGroup* m_group;
    ContentWidget* m_dateSettings;
};

#endif // DATETIME_H
