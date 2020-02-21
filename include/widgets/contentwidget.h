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

#ifndef CONTENTWIDGET_H
#define CONTENTWIDGET_H

#include <QWidget>
#include <QLabel>

#include <dimagebutton.h>

DWIDGET_USE_NAMESPACE

class QVBoxLayout;
class QScrollArea;
class QPropertyAnimation;
class QPushButton;
namespace dcc {

namespace widgets {
    class BackButton;
}

class ContentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContentWidget(QWidget *parent = 0);
    ~ContentWidget();

    void setTitle(const QString &title);
    void sendBackSignal();
    QWidget *content() const { return m_content; }
    QWidget *setContent(QWidget * const w);
    void scrollTo(int dy);

Q_SIGNALS:
    void back() const;
    void appear() const;
    void disappear() const;
    void wheelValueChanged() const;

protected:
    void resizeEvent(QResizeEvent *event) override;

protected:
    dcc::widgets::BackButton *m_backBtn;
    QLabel *m_title;
    QVBoxLayout *m_contentTopLayout;
    QScrollArea *m_contentArea;

    QWidget *m_content;
    double m_speedTime;
    int m_speed;
};

}

#endif // CONTENTWIDGET_H
