/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef THEMEITEM_H
#define THEMEITEM_H

#include "settingsitem.h"
#include "../../../widgets/labels/normallabel.h"
#include <QWidget>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QLabel>

using namespace dcc::widgets;

namespace dcc {
namespace personalization {
class ThemeItemPic;
class ThemeItem : public widgets::SettingsItem
{
    Q_OBJECT
public:
    explicit ThemeItem(const QJsonObject &json);

    void setTitle(const QString &title);
    void setSelected(bool selected);
    void setPic(const QString &picPath);
    const QString id() const;

protected:
    void mouseReleaseEvent(QMouseEvent *e);

signals:
    void selectedChanged(const bool selected) const;

private:
    QVBoxLayout *m_mainLayout;
    NormalLabel *m_title;
    QLabel *m_selectLabel;
    bool m_state;
    ThemeItemPic *m_itemPic;
    QString m_id;

};
}
}

#endif // THEMEITEM_H
