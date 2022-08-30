// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef UPDATEITEM_H
#define UPDATEITEM_H

#include "widgets/settingsitem.h"

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

    QSize sizeHint() const override;

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QString elidedChangelog() const;
    void updateChangelogDisplay();
    const QString getExtendLog() const;

private:
    AppUpdateInfo m_info;

    dcc::widgets::SmallLabel *m_appIcon;
    dcc::widgets::SmallLabel *m_appName;
    dcc::widgets::SmallLabel *m_appVersion;
    dcc::widgets::SmallLabel *m_appChangelog;
    bool m_isLogExpand;

    QPushButton *m_details;
    QVBoxLayout *m_iconLayout;
};

}
}
#endif // UPDATEITEM_H
