#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include "contentwidget.h"

class Frame;
class SettingsWidget : public ContentWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(Frame *frame);

private:
    QWidget *m_settingsWidget;
};

#endif // SETTINGSWIDGET_H
