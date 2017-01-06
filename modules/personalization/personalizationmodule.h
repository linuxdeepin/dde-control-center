#ifndef PERSONALIZATIONMODULE_H
#define PERSONALIZATIONMODULE_H

#include "moduleinterface.h"

namespace dcc
{
namespace personalization
{
class PersonalizationWidget;
class PersonalizationModel;
class PersonalizationWork;
class FontListWidget;
class FontsWidget;
class ThemeWidget;
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
    void showStanardFontsListWidget();
    void showMonoFontsListWidget();

private:
    ~PersonalizationModule();

    void contentPopped(ContentWidget *const w);

private:
    PersonalizationWidget *m_personalizationWidget;
    PersonalizationModel  *m_model;
    PersonalizationWork   *m_work;
    ThemeWidget           *m_themeWidget;
    FontsWidget           *m_fontSWidget;
    FontListWidget        *m_StandardfontList;
    FontListWidget        *m_MonofontList;
};
}
}


#endif // PERSONALIZATIONMODULE_H
