/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
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
