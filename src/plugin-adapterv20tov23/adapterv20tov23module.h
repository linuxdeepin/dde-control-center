//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef ADAPTERV20TOV23MODULE_H
#define ADAPTERV20TOV23MODULE_H

#include "interface/moduleobject.h"

class QEvent;
class QTimer;

class QHBoxLayout;
namespace dccV20 {
class ModuleInterface;
}

class AdapterV20toV23Module : public DCC_NAMESPACE::ModuleObject
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

    virtual QWidget *page() override;

Q_SIGNALS:
    void actived();

public:
    QString path() const;
    QString follow() const;
    bool enabled() const;

private:
    void setWidget(int index);

private:
    dccV20::ModuleInterface *m_v20Module;
    QList<QWidget *> m_widgets;
    QHBoxLayout *m_layout;
};

#endif // ADAPTERV20TOV23MODULE_H
