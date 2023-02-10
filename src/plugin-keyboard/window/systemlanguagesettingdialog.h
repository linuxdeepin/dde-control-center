//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SYSTEMLANGUAGESETTINGDIALOG_H
#define SYSTEMLANGUAGESETTINGDIALOG_H

#include "interface/namespace.h"
#include "widgets/buttontuple.h"
#include "src/plugin-keyboard/window/searchinput.h"

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

class SystemLanguageSettingDialog : public DAbstractDialog
{
    Q_OBJECT
public:
    explicit SystemLanguageSettingDialog(KeyboardModel *model, QWidget *parent = nullptr);
    ~SystemLanguageSettingDialog();

    void updateDataModel(QStandardItemModel *model, QModelIndex &selectedIndex, const QModelIndex &index);

    enum LanguageRole{
        TextRole = DTK_NAMESPACE::UserRole + 1,
        KeyRole,
        PingYinRole
    };

Q_SIGNALS:
    void click(const QModelIndex &index);

public Q_SLOTS:
    void setModelData(const QList<MetaData> &datas);
    void onSearch(const QString &text);
    void onAddLanguage();
    void onLangSelect(const QModelIndex &index);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

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
#endif // SYSTEMLANGUAGESETTINGDIALOG_H
