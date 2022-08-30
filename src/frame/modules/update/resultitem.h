// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DCC_UPDATE_RESULTITEM_H
#define DCC_UPDATE_RESULTITEM_H

#include "widgets/settingsitem.h"

#include "dimagebutton.h"
#include "common.h"

namespace dcc {
namespace widgets {
class NormalLabel;
}
}

namespace dcc {
namespace update {

class ResultItem : public dcc::widgets::SettingsItem
{
    Q_OBJECT

public:
    explicit ResultItem(QFrame* parent = 0);

    void setSuccess(ShowStatus type);
    void setMessage(const QString &message);

private:
    dcc::widgets::NormalLabel* m_message;
    QLabel *m_icon;
    QString m_pix;
};

} // namespace update
} // namespace dcc

#endif // DCC_UPDATE_RESULTITEM_H
