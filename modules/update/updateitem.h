#ifndef UPDATEITEM_H
#define UPDATEITEM_H

#include "settingsitem.h"

#include <types/appupdateinfolist.h>

#include <QLabel>

class QVBoxLayout;
class QPushButton;

namespace dcc {
namespace widgets {
class SmallLabel;
}
}

namespace dcc{
namespace update{

class UpdateItem : public dcc::widgets::SettingsItem
{
    Q_OBJECT

public:
    explicit UpdateItem(QFrame* parent = 0);

    void setAppInfo(const AppUpdateInfo& info);

    QString elideChangelog() const;
    int changelogLines() const;

private:
    AppUpdateInfo m_info;

    dcc::widgets::SmallLabel *m_appIcon;
    dcc::widgets::SmallLabel *m_appName;
    dcc::widgets::SmallLabel *m_appVersion;
    dcc::widgets::SmallLabel *m_appChangelog;

    QPushButton *m_details;
    QVBoxLayout *m_iconLayout;
};

}
}
#endif // UPDATEITEM_H
