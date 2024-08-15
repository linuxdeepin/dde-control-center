//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"
#include "widgets/settingsitem.h"
#include <QComboBox>

QT_BEGIN_NAMESPACE
class QStringList;
class QLabel;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {

class AlertComboBox;

class ComboxWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit ComboxWidget(QFrame *parent = nullptr);
    explicit ComboxWidget(const QString &title, QFrame *parent = nullptr);
    explicit ComboxWidget(QWidget *widget, QFrame *parent = nullptr);

    void setComboxOption(const QStringList &options);
    void setCurrentText(const QString &curText);
    void setCurrentIndex(const int index);
    void setTitle(const QString &title);

    AlertComboBox *comboBox();

Q_SIGNALS:
    void onIndexChanged(int index);
    void onSelectChanged(const QString &selected);
    void dataChanged(const QVariant &data);
    void clicked();

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QWidget *m_leftWidget;
    AlertComboBox *m_switchComboBox;
    QLabel *m_titleLabel;
    QString m_str;
};

class AlertComboBox : public QComboBox
{
    Q_OBJECT

    Q_PROPERTY(bool isWarning READ isWarning WRITE setIsWarning)

public:
    explicit AlertComboBox(QWidget *parent = Q_NULLPTR);
    ~AlertComboBox() override;
    void setIsWarning(bool isWarning);
    bool isWarning();
    bool eventFilter(QObject *o, QEvent *e) override;

Q_SIGNALS:
    void clicked();

protected Q_SLOTS:
    void onValueChange(const QString &text);

protected:
    void paintEvent(QPaintEvent *e) override;

private:
    bool m_isWarning;
};

}
