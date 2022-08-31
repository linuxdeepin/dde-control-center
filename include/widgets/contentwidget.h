// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
