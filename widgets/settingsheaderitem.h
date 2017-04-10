#ifndef SETTINGSHEADERITEM_H
#define SETTINGSHEADERITEM_H

#include "settingsitem.h"

#include <QHBoxLayout>
#include <QLabel>

namespace dcc {
namespace widgets {

class SettingsHeaderItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit SettingsHeaderItem(QWidget *parent = 0);

    QLabel *textLabel() const { return m_headerText; }
    QHBoxLayout *layout() const { return m_mainLayout; }

    void setTitle(const QString &title);
    void setRightWidget(QWidget *widget);

private:
    QHBoxLayout *m_mainLayout;
    QLabel *m_headerText;
};

}
}

#endif // SETTINGSHEADERITEM_H
