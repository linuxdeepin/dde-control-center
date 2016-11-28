#ifndef PERSONALIZATIONWIDGET_H
#define PERSONALIZATIONWIDGET_H

#include "modulewidget.h"
#include "contentwidget.h"
#include "settingsgroup.h"
#include "personalizationmodel.h"
#include "nextpagewidget.h"

#include <QPushButton>
#include <QVBoxLayout>

class PersonalizationWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit PersonalizationWidget();

signals:
    void showThemeWidget();
    void showFontsWidget();

private:
    dcc::SettingsGroup *m_userGroup;
};

#endif // PERSONALIZATIONWIDGET_H
