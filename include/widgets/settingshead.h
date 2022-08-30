// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SETTINGSHEAD_H
#define SETTINGSHEAD_H

#include "widgets/settingsitem.h"
#include "titlelabel.h"
#include <QPushButton>

DWIDGET_BEGIN_NAMESPACE
class DCommandLinkButton;
DWIDGET_END_NAMESPACE

namespace dcc {
namespace widgets {

class NormalLabel;
class SettingsHead : public SettingsItem
{
    Q_OBJECT

public:
    enum State {
        Edit,
        Cancel
    };

public:
    explicit SettingsHead(QFrame *parent = nullptr);

    void setTitle(const QString &title);
    void setEditEnable(bool state = true);

public Q_SLOTS:
    void toEdit();
    void toCancel();

Q_SIGNALS:
    void editChanged(bool edit);

private Q_SLOTS:
    void refershButton();
    void onClicked();

private:
    TitleLabel *m_title;
    DTK_WIDGET_NAMESPACE::DCommandLinkButton *m_edit;

    State m_state;
};

}
}

#endif // SETTINGSHEAD_H
