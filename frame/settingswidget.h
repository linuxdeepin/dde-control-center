#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include "contentwidget.h"

class SettingsWidget : public ContentWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = 0);

private:
    QWidget *m_settingsWidget;
};

#endif // SETTINGSWIDGET_H
