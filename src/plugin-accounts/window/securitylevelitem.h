//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCC_WIDGETS_SECURITYLEVEL_H
#define DCC_WIDGETS_SECURITYLEVEL_H

#include "interface/namespace.h"

#include <dtkwidget_global.h>

#include <QWidget>

#define PASSWORD_LEVEL_ICON_NUM 3
DWIDGET_BEGIN_NAMESPACE
class DLineEdit;
DWIDGET_END_NAMESPACE

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
class SecurityLevelItem : public QWidget
{
    Q_OBJECT
public:
    explicit SecurityLevelItem(QWidget *parent = nullptr);
    ~SecurityLevelItem();

    void setUser(const QString &userName);
    void bind(DTK_WIDGET_NAMESPACE::DLineEdit *lineEdit);

    enum Level {
        NoneLevel = 0,
        LowLevel,
        MidLevel,
        HighLevel
    };
    Q_ENUM(Level)

    enum IconType {
        NoneIcon = 0,
        RedIcon,
        YellowIcon,
        GreenIcon,
        ICONTYPE_NR_ITEMS
    };
    Q_ENUM(IconType)

public:
    void setLevel(Level level);

private:
    const QPixmap loadSvgImg(const QString &fileName, const int width, const int hight);
    void initUi();
    void initIcons();

private:
    QLabel *m_newPasswdLevelText;
    QLabel *m_newPasswdLevelIcons[PASSWORD_LEVEL_ICON_NUM];
    QPixmap m_icons[ICONTYPE_NR_ITEMS];
    Level m_level;
    QString m_userName;
};
} // namespace DCC_NAMESPACE

#endif // DCC_WIDGETS_SECURITYLEVEL_H
