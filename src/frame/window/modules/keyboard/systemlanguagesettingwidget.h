// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SYSTEMLANGUAGESETTINGWIDGET_H
#define SYSTEMLANGUAGESETTINGWIDGET_H

#include "interface/namespace.h"
#include "widgets/contentwidget.h"
#include "widgets/buttontuple.h"

#include <DGraphicsClipEffect>
#include <DListView>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace keyboard {
class LangWidget;
class KeyboardModel;
class IndexDelegate;
class MetaData;
class IndexModel;
class IndexView;
}

namespace widgets {
class SearchInput;
class TranslucentFrame;
class ButtonTuple;
}
}

namespace DCC_NAMESPACE {
namespace keyboard {
class SystemLanguageSettingWidget : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit SystemLanguageSettingWidget(dcc::keyboard::KeyboardModel *model, QWidget *parent = nullptr);
    ~SystemLanguageSettingWidget();

    void updateDataModel(QStandardItemModel *model, QModelIndex &selectedIndex, const QModelIndex &index);

    enum LanguageRole{
        TextRole = DTK_NAMESPACE::UserRole + 1,
        KeyRole,
        PingYinRole
    };

Q_SIGNALS:
    void click(const QModelIndex &index);
    void back();
public Q_SLOTS:
    void setModelData(const QList<dcc::keyboard::MetaData> &datas);
    void onSearch(const QString &text);
    void onAddLanguage();
    void onLangSelect(const QModelIndex &index);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QLabel *m_title;
    bool m_searchStatus;
    dcc::widgets::SearchInput *m_search;
    dcc::keyboard::KeyboardModel *m_keyboardModel;
    dcc::widgets::ButtonTuple *m_buttonTuple;
    Dtk::Widget::DListView *m_view;
    QStandardItemModel *m_model;
    QStandardItemModel *m_searchModel;
    DGraphicsClipEffect *m_clipEffectWidget;
    dcc::widgets::TranslucentFrame *m_contentWidget;
    QList<dcc::keyboard::MetaData> m_datas;  
    QModelIndex m_modelIndex;
    QModelIndex m_searchModelIndex;
};
}
}
#endif // SYSTEMLANGUAGESETTINGWIDGET_H
