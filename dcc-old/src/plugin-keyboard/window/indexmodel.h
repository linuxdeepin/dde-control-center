//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef INDEXMODEL_H
#define INDEXMODEL_H
#include "interface/namespace.h"
#include "operation/metadata.h"

#include <DListView>

#include <QString>
#include <QStandardItemModel>
#include <QItemDelegate>
#include <QFrame>

DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {

class IndexModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit IndexModel(QObject *parent = nullptr);

    void setMetaData(const QList<MetaData> &datas);
    QList<MetaData> metaData() const;
    int  indexOf(const MetaData &md);

    void setLetters(QList<QString> &letters);
    QList<QString> letters() const;
    int getModelCount();

protected:
    int rowCount(const QModelIndex &parent) const;
private:
    QList<MetaData> m_datas;
    QList<QString> m_letters;
public:
    enum {
        KBLayoutRole = Dtk::UserRole + 1,
    };
};

}
#endif // INDEXMODEL_H
