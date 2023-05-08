// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SYSTEMLANGUAGEWIDGET_H
#define SYSTEMLANGUAGEWIDGET_H

#include "dcclistview.h"
#include "interface/namespace.h"

#include <DCommandLinkButton>
#include <DFloatingButton>
#include <DFontSizeManager>
#include <DListView>

#include <QMouseEvent>
#include <QWidget>

DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {
class SystemLanguageSettingWidget;
class KeyboardModel;

class SystemLanguageListView : public DCCListView
{
    Q_OBJECT
public:
    explicit SystemLanguageListView(QWidget *parent = nullptr)
        : DCCListView(parent)
    {
        setSpacing(0);
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
    void addSystemLanguage();

private:
    KeyboardModel *m_model;
    QStringList m_sysLanglist;
    QStandardItemModel *m_langItemModel;
    SystemLanguageListView *m_langListview;
    DCommandLinkButton *m_editSystemLang;
    SystemLanguageSettingWidget *m_settingWidget;
    bool m_bEdit{ false };
    DViewItemAction *m_addLayoutAction;
};
} // namespace DCC_NAMESPACE
#endif // SYSTEMLANGUAGEWIDGET_H
