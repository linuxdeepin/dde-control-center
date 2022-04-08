/*
 * Copyright (C) 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <andywang_cm@deepin.com>
 *
 * Maintainer: andywang <andywang_cm@deepin.com>
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
#ifndef SYSTEMLANGUAGEWIDGET_H
#define SYSTEMLANGUAGEWIDGET_H

#include "interface/namespace.h"

#include <DCommandLinkButton>
#include <DListView>
#include <DFontSizeManager>
#include <DFloatingButton>

#include <QWidget>
#include <QMouseEvent>

DWIDGET_USE_NAMESPACE
namespace DCC_NAMESPACE {
class SystemLanguageSettingWidget;
class KeyboardModel;

class SystemLanguageListView : public DListView
{
    Q_OBJECT
public:
    explicit SystemLanguageListView(QWidget *parent = nullptr): DListView(parent) {}

protected:
    void mousePressEvent(QMouseEvent *event)
    {
        if(indexAt(event->pos()).row() == count() - 1)
            return;

        DListView::mousePressEvent(event);
    }
};

class SystemLanguageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SystemLanguageWidget(KeyboardModel *model, QWidget *parent = nullptr);
Q_SIGNALS:
    void languageAdded();
    void onSystemLanguageAdded();
    void delLocalLang(const QString &localLang);
    void setCurLang(const QString &curLang);
public Q_SLOTS:
    void onEditClicked();
    void onAddLanguage(const QString &localeLang);
    void onDefault(const QString &curLang);
    void setCurLangChecked(const QModelIndex &index);
    void onSetCurLang(int value);
private:
    KeyboardModel *m_model;
    QStringList m_sysLanglist;
    QStandardItemModel *m_langItemModel;
    SystemLanguageListView *m_langListview;
    DCommandLinkButton *m_editSystemLang;
    SystemLanguageSettingWidget *m_settingWidget;
    bool m_bEdit{false};
    DViewItemAction *m_addLayoutAction;
};
}
#endif // SYSTEMLANGUAGEWIDGET_H
