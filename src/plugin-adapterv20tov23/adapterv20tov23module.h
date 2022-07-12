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
#ifndef ADAPTERV20TOV23MODULE_H
#define ADAPTERV20TOV23MODULE_H

#include "interface/moduleobject.h"

class QEvent;
class QTimer;
namespace dccV20 {
class ModuleInterface;
}

class AdapterV20toV23Child : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    explicit AdapterV20toV23Child();
    ~AdapterV20toV23Child();

    virtual QWidget *page() override;
    void setPage(QWidget *w);

    inline QWidget *widget() const { return m_widget; }

private:
    QWidget *m_widget;
};

class AdapterV20toV23Module : public AdapterV20toV23Child
{
    Q_OBJECT
public:
    explicit AdapterV20toV23Module(dccV20::ModuleInterface *v20Module);
    ~AdapterV20toV23Module();

    virtual void active() override;
    virtual void deactive() override;

    dccV20::ModuleInterface *inter();
    void setChildPage(int level, QWidget *w);
    void popWidget(QWidget *w);

Q_SIGNALS:
    void actived();

public:
    QString path() const;
    QString follow() const;
    bool enabled() const;

private:
    dccV20::ModuleInterface *m_v20Module;
    QList<AdapterV20toV23Child *> m_modules;
};

#endif // ADAPTERV20TOV23MODULE_H
