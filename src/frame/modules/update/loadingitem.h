// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LOADINGITEM_H
#define LOADINGITEM_H

#include "widgets/settingsitem.h"
#include <QProgressBar>

namespace dcc {
namespace widgets {
class NormalLabel;
class LoadingIndicator;
}
}

namespace dcc {
namespace update {

class LoadingItem : public dcc::widgets::SettingsItem
{
    Q_OBJECT

public:
    explicit LoadingItem(QFrame* parent = 0);
    void setProgressValue(int value);
    void setProgressBarVisible(bool visible);
    void setMessage(const QString& message);

private:
    dcc::widgets::NormalLabel* m_messageLabel;
    QProgressBar *m_progress;
};

} // namespace widgets
} // namespace dcc

#endif // LOADINGITEM_H
