// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef EYECOMFORTMODEAPPLET_H
#define EYECOMFORTMODEAPPLET_H

#include "jumpsettingbutton.h"
#include "pluginlistview.h"

#include <QWidget>

#include <DSwitchButton>
#include <DLabel>

const QString THEME_LIGHT = "light";
const QString THEME_DARK = "dark";
const QString THEME_AUTO = "auto";

class EyeComfortmodeApplet : public QWidget
{
    Q_OBJECT

public:
    explicit EyeComfortmodeApplet(QWidget *parent = nullptr);
    void setEnabled(bool enable);
    void setTitle(const QString &title);
    void setIcon(const QIcon &icon);
    void setDescription(const QString &description);
    void setDccPage(const QString &first, const QString &second);
    void setEyeComfortVisible(bool visible);
    void setHeight(int height);

signals:
    void enableChanged(bool enable);
    void requestHideApplet();
    void globalThemeChanged(const QString &globalTheme);

public slots:
    void onGlobalThemeChanged(const QString &value);

private slots:
    void onThemeListClicked(const QModelIndex &index);

private:
    void initUi();
    void initConnect();
    void switchTheme(const QString &themeTypeName);

private:
    Dtk::Widget::DLabel *m_title;
    Dtk::Widget::DSwitchButton *m_switchBtn;
    JumpSettingButton *m_settingButton;
    PluginListView *m_themeView;
    QStandardItemModel *m_themeItemModel;
    PluginStandardItem *m_lightTheme;
    PluginStandardItem *m_darkTheme;
    PluginStandardItem *m_autoTheme;
    QString m_themeTypeName;
};

#endif // EYECOMFORTMODEAPPLET_H
