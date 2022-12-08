// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SYSTEMLANGUAGEWIDGET_H
#define SYSTEMLANGUAGEWIDGET_H

#include "interface/namespace.h"
#include "widgets/contentwidget.h"

#include <DCommandLinkButton>

#include <DListView>
#include <DFloatingButton>

#include <QWidget>

namespace dcc {
namespace keyboard {
class KeyboardModel;
class MetaData;
}

namespace widgets {
class TranslucentFrame;
}
}

namespace DCC_NAMESPACE {
namespace keyboard {
class SystemLanguageSettingWidget;
class SystemLanguageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SystemLanguageWidget(dcc::keyboard::KeyboardModel *model, QWidget *parent = nullptr);
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
    dcc::keyboard::KeyboardModel *m_model;
    QStringList m_sysLanglist;
    QStandardItemModel *m_langItemModel;
    DTK_WIDGET_NAMESPACE::DListView *m_langListview;
    DCommandLinkButton *m_editSystemLang;
    SystemLanguageSettingWidget *m_settingWidget;
    DFloatingButton *m_addSystemLanguage;
    bool m_bEdit{false};
};
}
}
#endif // SYSTEMLANGUAGEWIDGET_H
