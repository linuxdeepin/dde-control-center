#ifndef PERSONALIZATIONWIDGET_H
#define PERSONALIZATIONWIDGET_H

#include "modulewidget.h"
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

signals:
    void showThemeWidget();
    void showFontsWidget();

private:
    dcc::widgets::SettingsGroup *m_userGroup;
};
}
}


#endif // PERSONALIZATIONWIDGET_H
