// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef THEMEITEM_H
#define THEMEITEM_H

#include "widgets/settingsitem.h"
#include "widgets/labels/normallabel.h"
#include <QWidget>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QLabel>
#include <QVariant>

namespace dcc {
namespace widgets {
class ThemeItemPic;
class ThemeItem : public widgets::SettingsItem
{
    Q_OBJECT
public:
    explicit ThemeItem(QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setSelected(bool selected);
    void setPic(const QString &picPath);
    void setId(const QVariant &id);
    const QVariant id() const { return m_id; }

protected:
    void mouseReleaseEvent(QMouseEvent *e);

Q_SIGNALS:
    void selectedChanged(const bool selected) const;

private:
    QVBoxLayout *m_mainLayout;
    NormalLabel *m_title;
    QLabel *m_selectLabel;
    bool m_state;
    ThemeItemPic *m_itemPic;
    QVariant m_id;
};
}
}

#endif // THEMEITEM_H
