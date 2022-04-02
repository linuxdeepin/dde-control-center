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
#include "widgets/buttontuple.h"
#include "src/plugin-keyboard/window/searchinput.h"
#include "src/plugin-keyboard/window/indexmodel.h"

#include <DGraphicsClipEffect>
#include <DListView>
#include <DAbstractDialog>

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QEvent>

DWIDGET_USE_NAMESPACE
namespace DCC_NAMESPACE {
class ButtonTuple;

class KeyboardModel;
class MetaData;
class IndexModel;
class IndexView;

class SystemLanguageSettingWidget : public DAbstractDialog
{
    Q_OBJECT
public:
    explicit SystemLanguageSettingWidget(KeyboardModel *model, QWidget *parent = nullptr);
    ~SystemLanguageSettingWidget();

    void updateDataModel(QStandardItemModel *model, QModelIndex &selectedIndex, const QModelIndex &index);

    enum LanguageRole{
        TextRole = DTK_NAMESPACE::UserRole + 1,
        KeyRole,
        PingYinRole
    };

Q_SIGNALS:
    void click(const QModelIndex &index);
    void requestCloseDlg();

public Q_SLOTS:
    void setModelData(const QList<MetaData> &datas);
    void onSearch(const QString &text);
    void onAddLanguage();
    void onLangSelect(const QModelIndex &index);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    bool m_searchStatus;
    SearchInput *m_search;
    KeyboardModel *m_keyboardModel;
    ButtonTuple *m_buttonTuple;
    Dtk::Widget::DListView *m_view;
    QStandardItemModel *m_model;
    QStandardItemModel *m_searchModel;
    QList<MetaData> m_datas;
    QModelIndex m_modelIndex;
    QModelIndex m_searchModelIndex;
};
}
#endif // SYSTEMLANGUAGESETTINGWIDGET_H
