#ifndef PERSONALIZATIONMODULE_H
#define PERSONALIZATIONMODULE_H

#include "moduleinterface.h"
#include "personalizationwidget.h"
#include "personalizationmodel.h"
#include "personalizationwork.h"

#include "widget/themewidget/themewidget.h"
#include "widget/fontswidget/fontswidget.h"

namespace dcc
{
namespace personalization
{
class PersonalizationModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit PersonalizationModule(FrameProxyInterface *frame, QObject *parent = 0);

    void initialize();
    void moduleActive();
    void moduleDeactive();
    void reset();

    ModuleWidget *moduleWidget();

    const QString name() const;

private:
    void showThemeWidget();
    void showFontsWidget();

private:
    ~PersonalizationModule();

    void contentPopped(ContentWidget *const w);

private:

    PersonalizationWidget *m_PersonalizationWidget;
    PersonalizationModel  *m_model;
    PersonalizationWork   *m_work;
};
}
}


#endif // PERSONALIZATIONMODULE_H
