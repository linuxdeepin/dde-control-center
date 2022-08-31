// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
