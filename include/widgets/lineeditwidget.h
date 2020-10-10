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

#ifndef LINEEDITWIDGET_H
#define LINEEDITWIDGET_H

#include "widgets/settingsitem.h"

#include <QLabel>
#include <QLineEdit>
#include <DLineEdit>
#include <QHBoxLayout>
#include <darrowrectangle.h>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace widgets {

class ErrorTip : public DArrowRectangle {
    Q_OBJECT
public:
    explicit ErrorTip(QWidget *parent = nullptr);

    void setText(QString text);
    void clear();
    bool isEmpty() const;

public Q_SLOTS:
    void appearIfNotEmpty();

private:
    QLabel *m_label;
};

class LineEditWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit LineEditWidget(QFrame *parent = nullptr);
    explicit LineEditWidget(bool isPasswordMode, QWidget *parent = nullptr);

    inline QLineEdit *textEdit() const { return m_edit->lineEdit(); }
    inline DLineEdit *dTextEdit() const { return m_edit; }
    inline QString text() const { return m_edit->text(); }
    void setTitleVisible(const bool visible) { m_title->setVisible(visible); }
    void addRightWidget(QWidget *widget);
    void setReadOnly(const bool state);
    void setIsErr(const bool err = true);

    bool isShowAlert() { return m_errTip->isVisible(); }
    void showAlertMessage(const QString &message);
    void hideAlertMessage();

    inline QLabel *label() { return m_title; }

public Q_SLOTS:
    void setTitle(const QString &title);
    void setText(const QString &text);
    void setPlaceholderText(const QString &text);

protected:
    void mousePressEvent(QMouseEvent *e);

protected:
    QHBoxLayout *m_mainLayout;

private:
    QLabel *m_title;
    DLineEdit *m_edit;
    ErrorTip *m_errTip;
};

}
}

#endif // LINEEDITWIDGET_H
