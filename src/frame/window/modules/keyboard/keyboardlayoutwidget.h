// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef KEYBOARDLAYOUTWIDGET_H
#define KEYBOARDLAYOUTWIDGET_H

#include "interface/namespace.h"
#include "widgets/contentwidget.h"
#include "modules/keyboard/indexmodel.h"
#include "modules/keyboard/indexview.h"
#include "modules/keyboard/indexframe.h"
#include "modules/keyboard/indexdelegate.h"
#include "widgets/searchinput.h"
#include "widgets/buttontuple.h"

#include <DGraphicsClipEffect>
#include <DListView>

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

    void setDataModel(IndexModel *model, QModelIndex &selectedIndex, const QModelIndex &index);
    void setMetaData(const QList<MetaData>& datas);
    void setLetters(QList<QString> letters);

protected:
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void layoutSelected(const QString &value);

public Q_SLOTS:
    void onSearch(const QString& text);
    void onItemClicked(const QModelIndex &index);
    void onKBLayoutSelect(const QModelIndex &index);
    void onAddKBLayout();
private:
    int textLength;
    bool searchStatus;
    SearchInput *m_search;
    ButtonTuple *m_buttonTuple;
    IndexView *m_view;
    IndexModel *m_model;
    IndexModel *m_searchModel;
    IndexFrame *m_indexframe;
    TranslucentFrame *m_mainWidget;
    DGraphicsClipEffect *m_clipEffectWidget;
    QList<MetaData> m_data;
    QModelIndex m_selectIndex;
    QModelIndex m_selectSearchIndex;
};
}
}
#endif // KEYBOARDLAYOUTWIDGET_H
