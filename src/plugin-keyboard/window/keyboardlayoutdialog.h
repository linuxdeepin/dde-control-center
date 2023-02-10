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

#ifndef KEYBOARDLAYOUTDIALOG_H
#define KEYBOARDLAYOUTDIALOG_H

#include "indexmodel.h"
#include "indexview.h"
#include "interface/namespace.h"
#include "searchinput.h"
#include "widgets/buttontuple.h"

#include <DAbstractDialog>
#include <DGraphicsClipEffect>
#include <DListView>

#include <QWidget>

class QLineEdit;

DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {

class KeyboardLayoutDialog : public DAbstractDialog
{
    Q_OBJECT

public:
    explicit KeyboardLayoutDialog(QWidget *parent = nullptr);
    ~KeyboardLayoutDialog();

    void setDataModel(IndexModel *model, QModelIndex &selectedIndex, const QModelIndex &index);
    void setMetaData(const QList<MetaData> &datas);
    void setLetters(QList<QString> letters);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

Q_SIGNALS:
    void layoutSelected(const QString &value);

public Q_SLOTS:
    void onSearch(const QString &text);
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
} // namespace DCC_NAMESPACE
#endif // KEYBOARDLAYOUTDIALOG_H
