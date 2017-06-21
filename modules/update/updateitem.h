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

    static bool isAnchor(const QString &input);
    static QPair<QString, QString> parseAnchor(const QString &input);

private:
    AppUpdateInfo m_info;

    QString m_anchorName;
    QString m_anchorAddress;

    dcc::widgets::SmallLabel *m_appIcon;
    dcc::widgets::SmallLabel *m_appName;
    dcc::widgets::SmallLabel *m_appVersion;
    dcc::widgets::SmallLabel *m_appChangelog;

    QPushButton *m_details;
    QPushButton *m_openWebsite;
    QVBoxLayout *m_iconLayout;

    QString elidedChangelog() const;
    void expandChangelog();
};

}
}
#endif // UPDATEITEM_H
