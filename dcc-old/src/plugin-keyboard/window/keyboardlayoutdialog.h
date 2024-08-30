//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef KEYBOARDLAYOUTDIALOG_H
#define KEYBOARDLAYOUTDIALOG_H

#include "interface/namespace.h"
#include "widgets/buttontuple.h"
#include "indexmodel.h"
#include "indexview.h"
#include "searchinput.h"

#include <DGraphicsClipEffect>
#include <DListView>
#include <DAbstractDialog>

#include <QWidget>

class QLineEdit;

DWIDGET_USE_NAMESPACE
namespace DCC_NAMESPACE{

class KeyboardLayoutDialog : public DAbstractDialog
{
    Q_OBJECT

public:
    explicit KeyboardLayoutDialog(QWidget *parent = nullptr);
    ~KeyboardLayoutDialog();

    void setDataModel(IndexModel *model, QModelIndex &selectedIndex, const QModelIndex &index);
    void setMetaData(const QList<MetaData>& datas);
    void setLetters(QList<QString> letters);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

Q_SIGNALS:
    void layoutSelected(const QString &value);

public Q_SLOTS:
    void onSearch(const QString& text);
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
    DGraphicsClipEffect *m_clipEffectWidget;
    QList<MetaData> m_data;
    QModelIndex m_selectIndex;
    QModelIndex m_selectSearchIndex;
};
}
#endif // KEYBOARDLAYOUTDIALOG_H
