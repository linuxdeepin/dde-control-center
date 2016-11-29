#ifndef RESOLUTIONDETAILPAGE_H
#define RESOLUTIONDETAILPAGE_H

#include "contentwidget.h"
#include "settingsgroup.h"
#include "optionitem.h"
#include "displaymodel.h"

using dcc::SettingsGroup;

class ResolutionDetailPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit ResolutionDetailPage(QWidget *parent = 0);

    void setModel(DisplayModel *model);

private:
    DisplayModel *m_model;
    dcc::SettingsGroup *m_resolutions;

    QMap<int, dcc::OptionItem *> m_options;
};

#endif // RESOLUTIONDETAILPAGE_H
