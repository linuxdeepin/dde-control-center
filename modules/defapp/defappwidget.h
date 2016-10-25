#ifndef DEFAPP_H
#define DEFAPP_H

#include "modulewidget.h"
#include "contentwidget.h"
#include "settingsgroup.h"

#include <QPushButton>
#include <QVBoxLayout>

class DefaultAppsWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit DefaultAppsWidget();

signals:
    void showDefaultAppsDetail() const;

private:
    dcc::SettingsGroup  *m_userGroup;
    QPushButton         *m_nextPage;
};


#endif // DEFAPP_H
