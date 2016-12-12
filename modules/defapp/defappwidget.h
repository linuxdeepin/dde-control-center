#ifndef DEFAPP_H
#define DEFAPP_H

#include "modulewidget.h"
#include "contentwidget.h"
#include "settingsgroup.h"

#include <QPushButton>
#include <QVBoxLayout>

class DefaultAppsWidget : public dcc::ModuleWidget
{
    Q_OBJECT

public:
    explicit DefaultAppsWidget();

signals:
    void showDefaultAppsDetail() const;

private:
    dcc::widgets::SettingsGroup  *m_userGroup;
    QPushButton         *m_nextPage;
};


#endif // DEFAPP_H
