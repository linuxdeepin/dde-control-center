// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef NEXTPAGEWIDGET_H
#define NEXTPAGEWIDGET_H

#include "widgets/settingsitem.h"
#include "widgets/labels/normallabel.h"
#include "widgets/labels/tipslabel.h"
#include "nextbutton.h"

#include <QLabel>

namespace dcc {
namespace widgets {

class NextPageWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit NextPageWidget(QFrame *parent = nullptr, bool bLeftInterval = true);

    inline QString title() const { return m_title->text(); }
    void setTitle(const QString &title);
    void setValue(const QString &value);
    void setIcon(const QPixmap &icon);
    void setBtnHiden(const bool hiden);
    void setIconIcon(const QPixmap &icon);

    void clearValue() { m_value->clear(); }
    inline QString value() { return m_value->text();}

    void setRightTxtWordWrap(bool state = false);

protected:
    void mouseReleaseEvent(QMouseEvent *e);

Q_SIGNALS:
    void clicked() const;
    void selected() const;
    void acceptNextPage() const;

protected:
    NormalLabel *m_title;
    TipsLabel *m_value;
    NextButton *m_nextPageBtn;
    TipsLabel *m_icon;
};

}
}
#endif // NEXTPAGEWIDGET_H
