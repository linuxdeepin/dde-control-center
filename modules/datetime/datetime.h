#ifndef DATETIME_H
#define DATETIME_H

#include "datesettings.h"
#include "contentwidget.h"
#include "datetimeutil.h"
#include "chosedialog.h"

#include "modulewidget.h"
#include "settingsgroup.h"
#include "settingsitem.h"
#include "settingshead.h"
#include "nextpagewidget.h"

#include <QSettings>
using namespace dcc;

class Datetime : public ModuleWidget
{
    Q_OBJECT

public:
    explicit Datetime();
    ~Datetime();

signals:
    void editChanged(bool edit);
    void editDatetime();
    void addClick();

public slots:
    void addTimezone(const Timezone& tz);
    void slotClick();
    void slotEditMode(bool edit);
    void slotRemoveTimezone(const Timezone& tz);

private:
    bool m_bEdit;
    QSettings *m_settings;
    QList<Timezone> m_addeds;
    SettingsHead* m_headItem;
    NextPageWidget* m_addItem;
    SettingsGroup* m_group;
    ContentWidget* m_dateSettings;
};

#endif // DATETIME_H
