/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef COMBOBOXWIDGET_H
#define COMBOBOXWIDGET_H

#include "widgets/nextpagewidget.h"

#include <QPointer>

namespace dcc {

class ContentWidget;

namespace widgets {

class SettingsGroup;
class LineEditWidget;
class OptionItem;
class ComboBoxWidget : public NextPageWidget
{
    Q_OBJECT

public:
    explicit ComboBoxWidget(QFrame *parent = 0);

    void appendOption(const QString &name, const QVariant &value);
    void removeOption(const QVariant &value);
    void setCurrent(const QVariant &value);
    void setEditable(const bool editable = true);

Q_SIGNALS:
    void requestPage(ContentWidget * const page) const;
    void dataChanged(const QVariant &data) const;

private:
    // block parent's signal
    void acceptNextPage() const;
    void setValue(const QString &);

private Q_SLOTS:
    void onNextPageClicked();
    void onContentDesktory();
    void onItemClicked();

private:
    LineEditWidget *m_custom;
    SettingsGroup *m_optionsGroup;
    QPointer<ContentWidget> m_contentPage;

    OptionItem *m_lastSelectedItem;
    QMap<OptionItem *, QVariant> m_options;
};

}

}

#endif // COMBOBOXWIDGET_H
