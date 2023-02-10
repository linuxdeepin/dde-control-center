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

#ifndef INDEXMODEL_H
#define INDEXMODEL_H
#include "interface/namespace.h"
#include "operation/metadata.h"

#include <DListView>

#include <QFrame>
#include <QItemDelegate>
#include <QStandardItemModel>
#include <QString>

DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {

class IndexModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit IndexModel(QObject *parent = nullptr);

    void setMetaData(const QList<MetaData> &datas);
    QList<MetaData> metaData() const;
    int indexOf(const MetaData &md);

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

} // namespace DCC_NAMESPACE
#endif // INDEXMODEL_H
