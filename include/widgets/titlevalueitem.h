// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef TITLEVALUEITEM_H
#define TITLEVALUEITEM_H

#include "widgets/settingsitem.h"

#include <DTipLabel>

#include <QLabel>

DWIDGET_USE_NAMESPACE

QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE

namespace dcc
{

namespace widgets
{

class ResizeEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit ResizeEventFilter(QObject* parent = nullptr);
protected:
    bool eventFilter(QObject *watched, QEvent *event);
};

class TipsLabel;
class ItemTitleTipsLabel;

class TitleValueItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit TitleValueItem(QFrame *parent = nullptr);
    ~TitleValueItem() override;
    void setTitle(const QString& title);
    void setValue(const QString& value);
    void setWordWrap(const bool enable);
    QString value() const;
    void setValueAligment(const Qt::Alignment aligment);
    void setValueBackground(bool showBackground);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    TipsLabel* m_title;
    ItemTitleTipsLabel *m_value;
};


class TitleAuthorizedItem: public SettingsItem
{
    Q_OBJECT

public:
    TitleAuthorizedItem(QFrame *parent = nullptr);
    void setTitle(const QString& title);
    void setValue(const QString& value);
    void setWordWrap(bool enable);
    void setButtonText(const QString &str);
    void setValueForegroundRole(const QColor &color);
    void setVisable(bool value);

Q_SIGNALS:
    //传递button的点击信号
    void clicked();

private:
    TipsLabel* m_title;
    DTK_WIDGET_NAMESPACE::DTipLabel *m_value;
    QPushButton *m_pActivatorBtn;
};

class ItemTitleTipsLabel : public DTK_WIDGET_NAMESPACE::DTipLabel
{
    Q_OBJECT

public:
    explicit ItemTitleTipsLabel(const QString &text = QString(), QWidget *parent = nullptr);
    ~ItemTitleTipsLabel() override;
    void addBackground();
    void removeBackground();
    bool hasBackground() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    bool m_hasBackground;
};

}

}
#endif // TITLEVALUEITEM_H
