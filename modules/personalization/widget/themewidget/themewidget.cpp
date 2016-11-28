#include "themewidget.h"

ThemeWidget::ThemeWidget(QObject *)
{
//    SettingsGroup *m_mainlayout = new SettingsGroup;

//    m_mainlayout->appendItem(windowTheme);

    QVBoxLayout *mainlayout = new QVBoxLayout;
    QWidget *widget = new QWidget;
    WindowTheme *windowTheme = new WindowTheme;
    mainlayout->addWidget(windowTheme);

    widget->setLayout(mainlayout);
    setContent(widget);
}
