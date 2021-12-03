/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
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

Q_SIGNALS:
    void requestNextPage(ContentWidget * const page) const;
    void requestFrameAutoHide(const bool autoHide) const;
    void editClicked();

private:
    SettingsGroup *m_group;
    QVBoxLayout *m_layout;
    SettingsHead *m_head;
};

class ActionButton : public QWidget
{
    Q_OBJECT

public:
    enum ActionType { Add = 0, Delete };

Q_SIGNALS:
    void clicked();

public:
    explicit ActionButton(ActionType type, QWidget *parent);
    ~ActionButton() override;

protected:
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *object, QEvent *event) override;

    QColor backgroundColor();

private:
    ActionType m_actionType;
    bool m_isMouseEnter;
};

#endif /* ABSTRACTSECTION_H */
