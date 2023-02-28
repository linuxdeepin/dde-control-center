// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SYSTEMLANGUAGEWIDGET_H
#define SYSTEMLANGUAGEWIDGET_H

#include "interface/namespace.h"
#include "pagemodule.h"

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
class KeyboardWorker;

class SystemLanguageListView : public DListView
{
    Q_OBJECT
public:
    explicit SystemLanguageListView(QWidget *parent = nullptr)
        : DListView(parent)
    {
    }

protected:
    void mousePressEvent(QMouseEvent *event)
    {
        if (indexAt(event->pos()).row() == count() - 1)
            return;

        DListView::mousePressEvent(event);
    }
};

class SystemLanguagePage : public PageModule
{
    Q_OBJECT
public:
    explicit SystemLanguagePage(KeyboardModel *model,
                                  KeyboardWorker *worker,
                                  QWidget *parent = nullptr);
Q_SIGNALS:
    void languageAdded();
    void onSystemLanguageAdded();
    void delLocalLang(const QString &localLang);
    void setCurLang(const QString &curLang);
    void addLanguage(const QString &localeLang);
    void defaultChanged(const QString &curLang);
    void editSystemLangeSetText(const QString &text);
    void editSystemLangeSetVisible(const bool visible);
public Q_SLOTS:
    void onEditClicked();

    void onSetCurLang(int value);
    void onPushSystemLanguageSetting();
    void onAddLocale(const QModelIndex &index);

private:
    DCommandLinkButton *initTitleBtn();
    SystemLanguageListView *initListView();
    void initUI();

private:
    KeyboardModel *m_model;
    KeyboardWorker *m_worker;
    QStringList m_sysLanglist;
    bool m_bEdit{ false };
};
} // namespace DCC_NAMESPACE
#endif // SYSTEMLANGUAGEWIDGET_H
