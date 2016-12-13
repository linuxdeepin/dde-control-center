#ifndef DEFAPP_H
#define DEFAPP_H

#include "modulewidget.h"
#include <QPushButton>
#include <QVBoxLayout>
namespace dcc {
namespace widgets {
class SettingsGroup;
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
    QPushButton         *m_nextPage;
};
}
}



#endif // DEFAPP_H
