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

#include "window/namespace.h"
#include "widgets/contentwidget.h"

#include <DGraphicsClipEffect>

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
}
}

namespace DCC_NAMESPACE {
namespace keyboard {
class SystemLanguageSettingWidget : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit SystemLanguageSettingWidget(dcc::keyboard::KeyboardModel *model, QWidget *parent = 0);

Q_SIGNALS:
    void click(const QModelIndex &index);

public Q_SLOTS:
    void onSearch(const QString &text);
    void setCurLang(const QString &lang);
    void setModelData(const QList<dcc::keyboard::MetaData> &datas);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    dcc::widgets::SearchInput *m_search;
    dcc::keyboard::IndexDelegate *m_delegate;
    dcc::keyboard::IndexView *m_view;
    dcc::keyboard::IndexModel *m_model;
    dcc::keyboard::IndexModel *m_searchModel;
    dcc::keyboard::KeyboardModel *m_keyboardModel;
    DGraphicsClipEffect *m_clipEffectWidget;
    dcc::widgets::TranslucentFrame *m_contentWidget;
    QList<dcc::keyboard::MetaData> m_datas;
};
}
}
#endif // SYSTEMLANGUAGESETTINGWIDGET_H
