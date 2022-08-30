// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef OPTIONITEM_H
#define OPTIONITEM_H

#include "widgets/settingsitem.h"
#include "widgets/labels/normallabel.h"

#include <QHBoxLayout>
#include <QPointer>

namespace dcc {
namespace widgets {

class OptionItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit OptionItem(QWidget *parent = 0);
    explicit OptionItem(QString title, bool selected, QWidget *parent = 0);

    void setTitle(const QString &title);
    const QString title() const { return m_title; }
    void setTitleFixedHeight(int height);
    void setTitleWidget(QWidget *titleWidget);
    void setContentWidget(QWidget *contentWidget);
    void setContentsMargins(int left, int top, int right, int bottom);

    inline bool selected() const { return m_selected; }
    void setSelected(bool selected);

Q_SIGNALS:
    void selectedChanged() const;

protected:
    void mouseReleaseEvent(QMouseEvent *e);

protected:
    QString m_title;
    bool m_selected;

    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_titleLayout;
    QWidget *m_titleFrame;
    QPointer<QWidget> m_titleWidget;
    QPointer<QWidget> m_contentWidget;
    QLabel *m_selectedIcon;
};

} // namespace widgets;
} // namespace dcc;

#endif // OPTIONITEM_H
