#ifndef DEFAPP_H
#define DEFAPP_H

#include "modulewidget.h"
#include <QPushButton>
#include <QVBoxLayout>
namespace dcc {
namespace widgets {
class SettingsGroup;
class NextPageWidget;
}
namespace defapp {
class DefaultAppsWidget : public dcc::ModuleWidget
{
    Q_OBJECT

public:
    explicit DefaultAppsWidget();

signals:
    void showDefaultAppsDetail() const;

private:
    widgets::SettingsGroup  *m_userGroup;
    widgets::NextPageWidget *m_nextPage;
};
}
}



#endif // DEFAPP_H
