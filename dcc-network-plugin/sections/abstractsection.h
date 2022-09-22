// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ABSTRACTSECTION_H
#define ABSTRACTSECTION_H

#include "widgets/settingshead.h"

namespace dcc {
  namespace widgets {
    class SettingsGroup;
  }
  class ContentWidget;
}
DWIDGET_BEGIN_NAMESPACE
class DStyleOptionButton;
DWIDGET_END_NAMESPACE

DWIDGET_USE_NAMESPACE

using namespace dcc;
using namespace dcc::widgets;

class QVBoxLayout;

class AbstractSection : public QFrame
{
    Q_OBJECT

public:
    explicit AbstractSection(QFrame *parent = nullptr);
    explicit AbstractSection(const QString &title, QFrame *parent = nullptr);
    virtual ~AbstractSection();

    virtual bool allInputValid() = 0;
    virtual void saveSettings() = 0;

    void setTitle(const QString &title);
    void appendItem(SettingsItem *item);
    void insertItem(int idx, SettingsItem *item);
    void removeItem(SettingsItem *item);
    void setSettingsHead(SettingsHead *settingsHead);
    int itemIndex(SettingsItem *item);

Q_SIGNALS:
    void requestNextPage(ContentWidget * const page) const;
    void requestFrameAutoHide(const bool autoHide) const;
    void editClicked();

private:
    SettingsGroup *m_group;
    QVBoxLayout *m_layout;
    SettingsHead *m_head;
};

#endif /* ABSTRACTSECTION_H */
