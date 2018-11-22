/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef KEYBOARDLAYOUTWIDGET_H
#define KEYBOARDLAYOUTWIDGET_H

#include "widgets/contentwidget.h"
#include "indexmodel.h"
#include "indexview.h"
#include "indexframe.h"
#include "indexdelegate.h"
#include "widgets/searchinput.h"

#include <DGraphicsClipEffect>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;

namespace dcc {
namespace keyboard{

class QLineEdit;
class KeyboardLayoutWidget : public ContentWidget
{
    Q_OBJECT

public:
    explicit KeyboardLayoutWidget(QWidget *parent = 0);
    ~KeyboardLayoutWidget();

    void setMetaData(const QList<MetaData>& datas);
    void setLetters(QList<QString> letters);

protected:
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void layoutSelected(const QString &value);

public Q_SLOTS:
    void onSearch(const QString& text);
    void onItemClicked(const QModelIndex &index);

private:
    int textLength;
    SearchInput* m_search;
    IndexDelegate* m_delegate;
    IndexView* m_view;
    IndexModel* m_model;
    IndexModel* m_searchModel;
    IndexFrame* m_indexframe;
    TranslucentFrame *m_mainWidget;
    DGraphicsClipEffect *m_clipEffectWidget;
};
}
}
#endif // KEYBOARDLAYOUTWIDGET_H
