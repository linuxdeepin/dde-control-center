// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef OPTIONWIDGET_H
#define OPTIONWIDGET_H

#include "widgets/settingsitem.h"
#include "widgets/labels/normallabel.h"
#include "../model/category.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QDebug>
#include <DIconButton>
#include <QJsonObject>
#include <QObject>

namespace dcc
{
namespace defapp
{
DWIDGET_USE_NAMESPACE
class OptionWidget : public widgets::SettingsItem
{
    Q_OBJECT
    Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY checkedChanged)

public:
    explicit OptionWidget(const bool state = false, QWidget *parent = 0);
    inline const App getItem() { return m_CategoryItem;}
    void setItem(const App &item);
    bool checked() const;
    void setCheckedIcon(const QPixmap &icon);
    bool userCheck() const;
    QHBoxLayout *layout();

public Q_SLOTS:
    void setChecked(const bool checked);
    void setDelete(const bool delchecked);
    void setMime(const QString &mime);

private:
    QHBoxLayout  *m_mainlayout;
    QLabel       *m_optionIcon;
    QLabel       *m_execPath;
    DIconButton *m_delete;
    QLabel       *m_checkedIconLabel;
    widgets::NormalLabel  *m_displayName;
    bool          m_checked           = false;
    bool          m_delchecked        = false;
    bool          m_userCheck         = false;
    QString       m_mime;
    App m_CategoryItem;

protected:
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void removeItem(const App &item);
    void setDefault();
    void checkedChanged(const bool checked) const;
};
}
}



#endif // OPTIONWIDGET_H
