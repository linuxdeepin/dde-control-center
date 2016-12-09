#ifndef UPDATEITEM_H
#define UPDATEITEM_H

#include "settingsitem.h"

#include <types/appupdateinfolist.h>
#include <QLabel>
#include <QPushButton>

using namespace dcc;
namespace dcc{
namespace update{

class UpdateItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit UpdateItem(QFrame* parent = 0);

    void setAppInfo(const AppUpdateInfo& info);

    QString elideChangelog() const;
    int changelogLines() const;

private:
    QLabel* m_appIcon;
    QLabel* m_appName;
    QLabel* m_appVersion;
    QLabel* m_appChangelog;

    QPushButton* m_details;

    AppUpdateInfo m_info;
};

}
}
#endif // UPDATEITEM_H
