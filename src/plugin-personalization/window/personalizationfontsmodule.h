/*
* Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#ifndef PERSONALIZATIONFONTSMODULE_H
#define PERSONALIZATIONFONTSMODULE_H
#include "interface/moduleobject.h"

DCC_BEGIN_NAMESPACE
class ComboxWidget;
DCC_END_NAMESPACE
class PersonalizationModel;
class PersonalizationWorker;
class FontModel;
class QStandardItemModel;

class PersonalizationFontsModule : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    explicit PersonalizationFontsModule(PersonalizationModel *model, PersonalizationWorker *work, QObject *parent = nullptr);
    virtual void active() override;
Q_SIGNALS:

public Q_SLOTS:
    void setStandList(const QList<QJsonObject> &list);
    void setMonoList(const QList<QJsonObject> &list);
    void setList(const QList<QJsonObject> &list, QStandardItemModel *model);
private:
    void initFontWidget(DCC_NAMESPACE::ComboxWidget *combox,FontModel *fontModel);
private:
    QStandardItemModel *m_standardModel;
    QStandardItemModel *m_monospacedModel;

    PersonalizationModel *m_model;
    PersonalizationWorker *m_work;
};

#endif // PERSONALIZATIONFONTSMODULE_H
