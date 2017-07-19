#ifndef PERSONALIZATIONWIDGET_H
#define PERSONALIZATIONWIDGET_H

#include "modulewidget.h"
#include "personalizationmodel.h"
#include "switchwidget.h"
#include <QPushButton>
#include <QVBoxLayout>

namespace dcc
{
namespace widgets {
class SettingsGroup;
}
namespace personalization
{
class PersonalizationWidget : public dcc::ModuleWidget
{
    Q_OBJECT

public:
    explicit PersonalizationWidget();

public slots:
    void setModel(PersonalizationModel *const model);

signals:
    void showThemeWidget();
    void showFontsWidget();
    void requestSwitchWM();

private:
    dcc::widgets::SettingsGroup *m_userGroup;
    dcc::widgets::SwitchWidget *m_wmSwitch;
};
}
}


#endif // PERSONALIZATIONWIDGET_H
